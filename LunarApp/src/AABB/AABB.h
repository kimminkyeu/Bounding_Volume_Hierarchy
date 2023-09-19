//
// Created by Minkyeu Kim on 8/20/23.
//

#ifndef SCOOP_AABB_H
#define SCOOP_AABB_H


#include <glm.hpp>
#include <iomanip>
#include <iostream>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>
#include <stack>
#include "LunarApp/src/AABB/Mesh.h"
#include "LunarApp/src/Thread/ThreadPool.h"
#include "Lunar/Core/Timer.h"

// TODO: 구현이 끝나면, Template으로 바꿀 수 있는 부분 체크하기. (glm::vec3, glm::vec2, custom vec2 ... etc)

// ********************************************************************
// NOTE: https://jacco.ompf2.com/  --> 이 사람 내용을 보면서 공부할 것!
// ***********************************************************************
// https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf --> 이건 참조용.
// https://mshgrid.com/2021/01/17/aabb-tree/ --> 이것도 참조용.
// ********************************************************************


struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 directionDivision; // = 1 / dir

	Ray() = delete;
	Ray(const glm::vec3& origin, const glm::vec3& direction)
		: origin(origin), direction(direction)
	{
		// NOTE: pre-calculation for division speed at BBox intersection
		directionDivision.x = 1 / direction.x;
		directionDivision.y = 1 / direction.y;
		directionDivision.z = 1 / direction.z;
	}
	Ray(const Ray& other)
		: origin(other.origin), direction(other.direction), directionDivision(other.directionDivision)
	{}
};

// https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf
// Given two bounding boxes we can compute the union with min and max operations.
// These can be made efficient using SIMD.
struct BoundingBox
{
	glm::vec3 m_LowerBound = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 m_UpperBound = glm::vec3(std::numeric_limits<float>::min());

	// compute the surface area of bounding box (표면적)
	// later used in Surface Area Huristic
	float SurfaceArea() const
	{
		glm::vec3 d = m_UpperBound - m_LowerBound;
		return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
	}

	// axis x=0 y=1 z=2
	float GetAxisLength(int axis) const
	{
		glm::vec3 d = m_UpperBound - m_LowerBound;
		return d[axis];
	}

	// box를 주어진 vertex를 포함하도록 키움.
	void Grow(const glm::vec3& v)
	{
		Grow(v.x, v.y, v.z);
	}

	void Grow(float x, float y, float z)
	{
		m_LowerBound = glm::vec3(std::min(m_LowerBound.x, x), std::min(m_LowerBound.y, y), std::min(m_LowerBound.z, z));
		m_UpperBound = glm::vec3(std::max(m_UpperBound.x, x), std::max(m_UpperBound.y, y), std::max(m_UpperBound.z, z));
	}

	void Grow(const BoundingBox& box)
	{
		Grow(box.m_LowerBound);
		Grow(box.m_UpperBound);
	}

	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html
	// https://youtu.be/TrqK-atFfWY?t=1349
	// NOTE: return hit BBox distance
	float Intersect(const Ray& ray) const // hitPoint = rayOrigin + rayDirection * t;  --> for every x, y, z plain, t must be the same.
	{
		// get t for x axis
		float tx1 = (m_LowerBound.x - ray.origin.x) * ray.directionDivision.x;
		float tx2 = (m_UpperBound.x - ray.origin.x) * ray.directionDivision.x;
		// get t for y axis
		float ty1 = (m_LowerBound.y - ray.origin.y) * ray.directionDivision.y;
		float ty2 = (m_UpperBound.y - ray.origin.y) * ray.directionDivision.y;
		// get t for z axis
		float tz1 = (m_LowerBound.z - ray.origin.z) * ray.directionDivision.z;
		float tz2 = (m_UpperBound.z - ray.origin.z) * ray.directionDivision.z;
		// get min, max for every t
		float txmin = glm::min(tx1, tx2); float txmax = glm::max(tx1, tx2);
		float tymin = glm::min(ty1, ty2); float tymax = glm::max(ty1, ty2);
		float tzmin = glm::min(tz1, tz2); float tzmax = glm::max(tz1, tz2);
		// get tmin, tmax
		float tmin = glm::max(txmin, tymin); tmin = glm::max(tmin, tzmin);
		float tmax = glm::min(txmax, tymax); tmax = glm::min(tmax, tzmax);

		/* TODO: Implement intersection pass logic
		 * 		https://www.youtube.com/shorts/GqwUHXvQ7oA
		 * 		https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html */
		// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
		if (tmax < 0.0f) return -1.0f;
		if (tmin > tmax) return -1.0f;
		if (tmin < 0.0f) return tmax;
		else			 return tmin;
	}
};

union TriangleDataFormat// VBO structure -> x, y, z, tx, ty, nx, ny, nz
{
	struct {
		float x;
		float y;
		float z;
		float tx; // uv
		float ty; // uv
		float nx; // normal x
		float ny; // normal y
		float nz; // normal z
	};
	float d[8];
	float& operator[] (int index)
	{
		if (index >= 0 && index < 8) return d[index];
		else throw std::out_of_range("TriangleDataFormat: index out of range");
	}
	inline glm::vec3 GetVertex() const { return { x, y, z }; }
	inline glm::vec2 GetUV() const { return { tx, ty }; }
	inline glm::vec3 GetNormal() const { return { nx, ny, nz }; }
};

template <typename T>
class Triangle // Triangle[0] = Triangle.v0
{
public:
	T v0 = T();
	T v1 = T();
	T v2 = T();
private:
	// if inf, then value is not set.
	glm::vec3 m_Centroid = glm::vec3(std::numeric_limits<float>::infinity());

public:
	Triangle() = default;

	Triangle(T _v0, T _v1, T _v2)
		: v0(_v0), v1(_v1), v2(_v2), m_Centroid()
	{ __CalculateCentroid(); }

	glm::vec3 GetCentroid()
	{
		if (m_Centroid == glm::vec3(std::numeric_limits<float>::infinity()))
		{
			__CalculateCentroid();
		}
		return m_Centroid;
	}

private:
	void __CalculateCentroid()
	{
		this->m_Centroid.x = (v0[0] + v1[0] + v2[0]) * 0.3333f;
		this->m_Centroid.y = (v0[1] + v1[1] + v2[1]) * 0.3333f;
		this->m_Centroid.z = (v0[2] + v1[2] + v2[2]) * 0.3333f;
	}

public:
	T& operator[] (int index)
	{
		if (index == 0) return v0;
		if (index == 1) return v1;
		if (index == 2) return v2;
		else throw std::out_of_range("Triangle: index out of range");
	}

	const T& operator[] (int index) const
	{
		if (index == 0) return v0;
		if (index == 1) return v1;
		if (index == 2) return v2;
		else throw std::out_of_range("Triangle: index out of range");
	}
};

struct Hit
{
	using triangle_type = Triangle<TriangleDataFormat>;
	float distance = -1.0f; // hit distance

	glm::vec3 point; // hit point
	glm::vec3 blendedPointNormal; // for ray-tracing. --> blend normal with Barycentric coord

	triangle_type triangle; // 충돌한 삼각형 3개의 각 vertex data.
	glm::vec3 faceNormal; // hit point normal
};

// ------------------------------------------------
// Implementation of Axis-aligned bounding box
struct AABBNode
{
	AABBNode() = default;

	AABBNode(unsigned int triStartIdx, size_t triCount)
		: m_TriangleStartIndex(triStartIdx), m_TriangeCount(triCount)
	{};

	BoundingBox m_Bounds; // bounding box of the node
	unsigned int m_Left = -1;
	unsigned int m_Right = -1;
	unsigned int m_TriangleStartIndex = 0; // store the index of the first primitive, and the number of primitives.
	size_t m_TriangeCount = 0; // Node가 가질 primitive 길이.

	inline bool IsLeaf() const noexcept
	{ return (m_TriangeCount > 0); }
};

// ------------------------------------------------
// Implementation of Axis-aligned bounding box
class AABBTree
{
public:
	unsigned int m_MaxDepth = 0; // 0 = root
private: // typedef and namespace scope
	using triangle_type = Triangle<TriangleDataFormat>;
	// ...
private: // member data
	std::vector<AABBNode> m_Nodes; // Node Pool
	unsigned int m_RootIndex = 0; // root index of node pool
	std::vector<triangle_type> m_Triangles; // primitive pool
	std::vector<size_t> m_TriangleIndexBuffer; // just like IBO, we change triangle sequence with this.
	size_t m_TotalTriangleCount;

	// NOTE: AABB Thread test
	mutable std::mutex m_TaskMutex; // bfs queue pop시 lock 걸기.
	ThreadPool m_ThreadPool;

	mutable std::mutex m_FinishedCountMutex; // 특정 노드가 leaf로 분할 종료될 경우, 해당 노드의 삼각형 count를 sum하여 main thread에서 감지하는 용도.
	size_t m_FinishedTrianglesCount = 0;

	mutable std::mutex m_NodeCountMutex; // 생성하는 노드 개수에 대한 ++ increment용 mutex (node_idx increment용)
	size_t m_NodeCount = 0; // 생성되는 node에 대한 count++



private: // member data tmp
	struct bbox_level_type
	{
		int level = -1;
		bool isLeaf = false;
	};
	using aabb_mesh_data_type = std::pair<std::shared_ptr<AABB::Mesh>, bbox_level_type>;
	std::vector<aabb_mesh_data_type> m_AABBMeshList;

private:
	// Subdivide space
	void __BuildBVH_TopDown()
	{
		Lunar::Timer timer;
		LOG_INFO("BVH build start");

		m_Nodes.clear();
		// to start, assign all triangles to root node.
		m_Nodes.emplace_back( 0, m_Triangles.size() ); // (0) insert node at root
		// update root node bound
		__UpdateNodeBounds(m_RootIndex); // (1) Update Each Bode Bound

#if (MT == 1) // NOTE: this variable is set by CMakelist.txt
		__SubdivideParallel(m_RootIndex);
#else
		// divide BBOX
		__SubdivideBFS(m_RootIndex); // (2) Subdivide space recursively
//		__Subdivide_recur_old(m_RootIndex); // (3) Deprecated version, just for performace test.
#endif
		LOG_INFO("BVH build finished at {0} ms", timer.ElapsedMillis());



		// create AABB mesh for visualization
//#if (MT == 1) //
//		__GenerateDebugMeshParallel(m_RootIndex); // NOTE: OpenGL call에 쓰레딩이 안되서 일단 사용 금지.
//#else
		__GenerateDebugMeshBFS(m_RootIndex); // (3) for debug render, generate mesh(VAO, VBO.. etc) for each Bounding Box;
//#endif
	}

	void __UpdateNodeBounds(unsigned int nodeIdx)
	{
		AABBNode* targetNode = &(m_Nodes[nodeIdx]);
		BoundingBox* bbox = &(targetNode->m_Bounds);
		const unsigned int start = targetNode->m_TriangleStartIndex;
		// update bounding box for node's every primitive
		for (unsigned int i = 0; i < targetNode->m_TriangeCount; ++i) {
			const auto triIdx = m_TriangleIndexBuffer[start + i];
			for (int v=0; v<3; ++v) {
				// 모든 삼각형의 모든 vertex를 돌면서, vec3 bound와 vec3 v0 중 최소값을 bound에 갱신.
				const auto vertex = (m_Triangles[triIdx])[v];
				bbox->m_LowerBound = glm::vec3(std::min(bbox->m_LowerBound.x, vertex.x), std::min(bbox->m_LowerBound.y, vertex.y), std::min(bbox->m_LowerBound.z, vertex.z));
				bbox->m_UpperBound = glm::vec3(std::max(bbox->m_UpperBound.x, vertex.x), std::max(bbox->m_UpperBound.y, vertex.y), std::max(bbox->m_UpperBound.z, vertex.z));
			}
		}
	}

	// NOTE:	[ 참고자료. Fast, Binnded BVH building ]
	//  		Reference : https://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf
	// 		    --------------------------------------------------------------------------------
	// 		 	( Option 1 : 위 논문에서는 bbox의 가장 긴 axis만 체크하였다)
	// 		 	"...place bins only along axis in which the centroids’ bounding box is widest.
	// 		 	Though checking all three axis in turn might yield even better results,
	// 		 	binning only along the dominant axis so far has produced quite reasonable results."
	// 		    --------------------------------------------------------------------------------
	// 		 	( Option 2 : 이건 내가 선택한 방법으로, 시간은 더 걸릴 지 모르나 (1)보다 나은 품질 획득 )
	// 		    --------------------------------------------------------------------------------
	//          추후 thread build를 진행할 때를 고려해서 수정할 것.

	struct Bin
	{
		BoundingBox bbox;
		int triangleCount = 0;
	};

	struct SplitEvaluationResult
	{
		int axis = -1; // x=0 y=1 z=2
		float position = 0; // split pos
		float cost = std::numeric_limits<float>::max(); // split cost (SAH)
	};

	struct SAHData
	{
		float leftSurfaceArea;
		float leftTriangleCount;
		float rightSurfaceArea;
		float rightTriangleCount;
	};

	// calcalate best split plane via SAH.
	// TODO: NUM_OF_BINS 변수에 따라 Tree 깊이가 점점 짧아지는 이유는?
	SplitEvaluationResult __FindBestSplitPlane(const AABBNode& node)
	{
		SplitEvaluationResult bestResult;
		const int NUM_OF_BINS = 8;
		const float NUM_OF_BINS_INVERSE = 1.0f / (float)NUM_OF_BINS;

		for (int currAxis = 0; currAxis < 3; ++currAxis)
		{
			// NOTE: 0. re-calculate split plane range with centroids.
			//          plain을 정할 때, 그 분할의 시작/끝을 AABB가 아닌 Centroid들 기준으로 잡는게 더 Compact하다.
			//          왜냐면 비어있는 bin이 적을 수록 좋기 때문.
			float boundsMin = std::numeric_limits<float>::max();
			float boundsMax = std::numeric_limits<float>::min();
			for (int i = 0; i < node.m_TriangeCount; ++i)
			{
				size_t triIdx = m_TriangleIndexBuffer[node.m_TriangleStartIndex + i];
				auto& triangle = m_Triangles[triIdx];
				boundsMin = std::min( boundsMin, triangle.GetCentroid()[currAxis] );
				boundsMax = std::max( boundsMax, triangle.GetCentroid()[currAxis] );
			}


			// NOTE: 1. prepare bins
			// 	     populate the bins by visiting the primitives once (per axis)
			Bin bins[NUM_OF_BINS];

			float scale = NUM_OF_BINS / (boundsMax - boundsMin);
			// 노드의 모든 삼각형에 대해, 하나씩 돌면서 bin의 크기를 설정.
			for (uint i = 0; i < node.m_TriangeCount; ++i)
			{
				size_t triIdx = m_TriangleIndexBuffer[node.m_TriangleStartIndex + i];
				auto& triangle = m_Triangles[triIdx];
				// 삼각형 centroid를 가지고, bin의 index를 계산해야 함.
				const float maxBinIdx = NUM_OF_BINS - 1;
				// centroid를 이용한 binxIdx 계산.
				const float calculateBinIdx = (triangle.GetCentroid()[currAxis] - boundsMin) * scale;
				const int binIdx = std::min(maxBinIdx, calculateBinIdx);// just for safety.
				bins[binIdx].triangleCount++;
				bins[binIdx].bbox.Grow(triangle.v0.GetVertex());
				bins[binIdx].bbox.Grow(triangle.v1.GetVertex());
				bins[binIdx].bbox.Grow(triangle.v2.GetVertex());
			}

			// NOTE: 2. gather data for the 7 planes between the 8 bis.
			// [ - - - - ]
			// [ - ] p0 [ - - - ] : bins[i] (i=0) NOTE: Left  sweep starts from here ↓↓↓
			// [ - - ] p1 [ - - ] : bins[i] (i=1)
			// [ - - - ] p2 [ - ] : bins[i] (i=2) NOTE: Right sweep starts from here ↑↑↑
			SAHData splitPlains[NUM_OF_BINS - 1];
			BoundingBox leftBox, rightBox;// 누적용 변수
			int leftSum = 0, rightSum = 0;// 누적용 변수
			// 양 끝에서 함께 확장되면서 SAH를 위한 값 동시 계산
			for (int i = 0; i < NUM_OF_BINS - 1; ++i)
			{
				leftSum += bins[i].triangleCount;
				splitPlains[i].leftTriangleCount = leftSum;// bbox 화장되면서 점점 누적.
				leftBox.Grow(bins[i].bbox);
				splitPlains[i].leftSurfaceArea = leftBox.SurfaceArea();

				rightSum += bins[NUM_OF_BINS - i - 1].triangleCount;
				splitPlains[NUM_OF_BINS - i - 2].rightTriangleCount = rightSum;
				rightBox.Grow(bins[NUM_OF_BINS - i - 1].bbox);
				splitPlains[NUM_OF_BINS - i - 2].rightSurfaceArea = rightBox.SurfaceArea();
			}

			// NOTE: 3. calculate SAH cost for the 7 planes
			const float UNIT_SCALE = (boundsMax - boundsMin) * NUM_OF_BINS_INVERSE;
			for (int i = 0; i < NUM_OF_BINS - 1; ++i)
			{
				float planeCost = splitPlains[i].leftSurfaceArea * splitPlains[i].leftTriangleCount \
								+ splitPlains[i].rightSurfaceArea * splitPlains[i].rightTriangleCount;
				if (planeCost < bestResult.cost)
				{
					bestResult.axis = currAxis;
					bestResult.position = node.m_Bounds.m_LowerBound[currAxis] + (UNIT_SCALE * float(i + 1));
					bestResult.cost = planeCost;
				}
			}
		}
		return bestResult;
	}

	// NOTE: 아래 방식의 top-Down은 루트에서 부터 내려가면서 생성됨으로 상단 노드에서 bottle-neck이 발생한다.
	// 		  즉 bottle-neck이 발생하는 쓰레드 방식이다!
	// 이 부분은 bottom up 방식으로 Tree를 생성하는 로직을 통해 해결할 수 있지 않을까?
	// 참고 : https://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf
	void __SubdivideParallel(unsigned int parentNodeIdx)
	{
		// NOTE: push_back이 thread-safe가 아니여서, 미리 메모리에 잡아두었다.
		m_Nodes.resize(m_TotalTriangleCount * 2 - 1);

		// lamda function (재귀 호출 구조)
		auto Subdivide_recur = [&](auto&& Subdivide_recur, unsigned int currIdx) -> void
		{
		  AABBNode& node = m_Nodes[currIdx];
//		  LOG_TRACE("Thread {0} is subdividing node {1}", std::this_thread::get_id(), currIdx);

		  // NOTE: find best split plane.
		  SplitEvaluationResult evaluationResult = __FindBestSplitPlane(node);
		  const int axis = evaluationResult.axis;
		  const float splitPos = evaluationResult.position;

		  // NOTE: check if the best split cost is actually an improvement over not splitting.
		  const float parentCost = node.m_TriangeCount * node.m_Bounds.SurfaceArea();
		  if (evaluationResult.cost >= parentCost)
		  {
			  // NOTE: 메인 쓰레드에서 각 쓰레드들이 모두 끝났음을 알려면, 이 return 조건에서 파악해야 한다.
			  {
				  std::lock_guard<std::mutex> lock(m_FinishedCountMutex);
				  m_FinishedTrianglesCount += node.m_TriangeCount;
			  }
			  return ; // stop Thread. finished.
		  }

		  // NOTE: split via best plane.
		  unsigned int startIdx = node.m_TriangleStartIndex;
		  unsigned int endIdx = startIdx + node.m_TriangeCount - 1;
		  while (startIdx <= endIdx)
		  {
			  // 해당 삼각형의 axis의 중심과 splitPos를 비교 // 삼각형 중심으로 분할하기 때문에, 겹치는 영역이 발생.
			  const auto triIdx = m_TriangleIndexBuffer[startIdx];
			  // 잠깐... m_Triangles는 일단 공유하는 애잖아...
			  const auto centerOfTargetAxisPrimitive = (m_Triangles[triIdx].GetCentroid())[axis];
			  if (centerOfTargetAxisPrimitive < splitPos) {
				  startIdx++;
			  } else { // same as quick-sort pivot move
				  std::swap(m_TriangleIndexBuffer[startIdx], m_TriangleIndexBuffer[endIdx--]);
			  }
		  }

		  // abort split if one of the sides is empty
		  size_t leftCount = startIdx - node.m_TriangleStartIndex;
		  if (leftCount == 0 || leftCount == node.m_TriangeCount)
		  {
			  // NOTE: 메인 쓰레드에서 각 쓰레드들이 모두 끝났음을 알려면, 이 return 조건에서 파악해야 한다.
			  {
				  std::lock_guard<std::mutex> lock(m_FinishedCountMutex);
				  m_FinishedTrianglesCount += node.m_TriangeCount;
			  }
			  return ; // Stop Thread
		  };

		  unsigned int leftChildIdx;
		  unsigned int rightChildIdx;
		  {
			  // NOTE: 이 부분이 정말 중요! node에 push_back으로 사이즈를 체크하는게 아니기 때문에, 각 쓰레드에서 이 부분을 인지하고 +2 카운트.
			  std::lock_guard<std::mutex> lock(m_NodeCountMutex);
			  leftChildIdx = m_NodeCount + 1;
			  rightChildIdx = m_NodeCount + 2;
			  m_NodeCount += 2;
		  }
		  // insert left
		  node.m_Left = leftChildIdx;
		  m_Nodes[leftChildIdx].m_TriangleStartIndex = node.m_TriangleStartIndex;
		  m_Nodes[leftChildIdx].m_TriangeCount = leftCount;
		  __UpdateNodeBounds(leftChildIdx);

		  // insert right
		  node.m_Right = rightChildIdx;
		  m_Nodes[rightChildIdx].m_TriangleStartIndex = startIdx;
		  auto k = node.m_TriangeCount;
		  m_Nodes[rightChildIdx].m_TriangeCount = node.m_TriangeCount - leftCount;
		  __UpdateNodeBounds(rightChildIdx);

		  // set prim count to 0, because it's not a leaf node anymore.
		  node.m_TriangeCount = 0;

		  // add child idx to Task Queue
		  {
			  std::lock_guard<std::mutex> lock(m_TaskMutex);
			  m_ThreadPool.AddTask(Subdivide_recur, Subdivide_recur, leftChildIdx);
			  m_ThreadPool.AddTask(Subdivide_recur, Subdivide_recur, rightChildIdx);
		  }
		};
		m_ThreadPool.AddTask(Subdivide_recur, Subdivide_recur, m_RootIndex);

		// NOTE: 람다가 재귀 호출되면서 task-queue에 데이터가 호출되는 방식인데... main thread에서 어떤 기준으로 wait하지...?
		size_t triCountSum = 0;
		while (triCountSum < m_TotalTriangleCount)
		{
			{
				std::lock_guard<std::mutex> lock(m_FinishedCountMutex);
				triCountSum = m_FinishedTrianglesCount;
			}
		}
	}

	void __SubdivideBFS(unsigned int parentNodeIdx)
	{
		std::queue<unsigned int> Queue;

		Queue.push(parentNodeIdx);
		while (!Queue.empty())
		{
			unsigned int currIdx = Queue.front(); Queue.pop();
			AABBNode& node = m_Nodes[currIdx];

			// NOTE: find best split plane.
			SplitEvaluationResult evaluationResult = __FindBestSplitPlane(node);
			const int axis = evaluationResult.axis;
			const float splitPos = evaluationResult.position;

			// NOTE: check if the best split cost is actually an improvement over not splitting.
			const float parentCost = node.m_TriangeCount * node.m_Bounds.SurfaceArea();
			if (evaluationResult.cost >= parentCost) {
				continue ; // stop BFS
			}

			// NOTE: split via best plane.
			unsigned int startIdx = node.m_TriangleStartIndex;
			unsigned int endIdx = startIdx + node.m_TriangeCount - 1;
			while (startIdx <= endIdx)
			{
				// 해당 삼각형의 axis의 중심과 splitPos를 비교 // 삼각형 중심으로 분할하기 때문에, 겹치는 영역이 발생.
				const auto triIdx = m_TriangleIndexBuffer[startIdx];
				const auto centerOfTargetAxisPrimitive = (m_Triangles[triIdx].GetCentroid())[axis];
				if (centerOfTargetAxisPrimitive < splitPos) {
					startIdx++;
				} else { // same as quick-sort pivot move
					std::swap(m_TriangleIndexBuffer[startIdx], m_TriangleIndexBuffer[endIdx--]);
				}
			}

			// abort split if one of the sides is empty
			size_t leftCount = startIdx - node.m_TriangleStartIndex;
			if (leftCount == 0 || leftCount == node.m_TriangeCount) {
				continue ; // continue to next bfs
			};

			// Create child nodes.
			// ------------------------------
			// quick sort partitioning과 유사함.
			// 따라서, [ 0 .... startIdx - 1 | startIdx .... N ] 이렇게 나눠짐.
			// AABB indexing (idx)
			//                         0
			//              1                    2
			//          3       4            9      10
			//        5   6   7   8       11  12   13  14
			//      ...                                  ...

			// insert left
			unsigned int leftChildIdx = m_Nodes.size();  // if 1
			node.m_Left = leftChildIdx;
			m_Nodes.emplace_back(node.m_TriangleStartIndex, leftCount);
			__UpdateNodeBounds(leftChildIdx);

			// insert right
			unsigned int rightChildIdx = m_Nodes.size(); // then 2
			node.m_Right = rightChildIdx;
			m_Nodes.emplace_back(startIdx, node.m_TriangeCount - leftCount);
			__UpdateNodeBounds(rightChildIdx);

			// set prim count to 0, because it's not a leaf node anymore.
			node.m_TriangeCount = 0;

			// child BFS
			Queue.push(leftChildIdx);
			Queue.push(rightChildIdx);
		}
	}



	// NOTE: -------------------- OLD VERSION -----------------------------------------------------
	// Surface Area Huristic, but slow version
	float ComputeCostbySAH_old(AABBNode& node, int axis, float splitPos)
	{
		// determine triangle counts and bounds.
		BoundingBox leftBox, rightBox;
		int leftCount = 0, rightCount = 0;
		// 모든 노드 내 삼각형에 대해 주어진 pos 기준 왼쪽, 오른쪽인지 구분.
		for (size_t i=0; i < node.m_TriangeCount; ++i)
		{
			auto triIdx = m_TriangleIndexBuffer[node.m_TriangleStartIndex + i];
			auto &triangle = m_Triangles[triIdx];
			const auto centroid = (triangle.v0[axis] + triangle.v1[axis] + triangle.v2[axis]) * 0.3333f;
			if (centroid < splitPos)
			{
				++leftCount;
				leftBox.Grow(triangle.v0.x, triangle.v0.y, triangle.v0.z);
				leftBox.Grow(triangle.v1.x, triangle.v1.y, triangle.v1.z);
				leftBox.Grow(triangle.v2.x, triangle.v2.y, triangle.v2.z);
			}
			else
			{
				++rightCount;
				rightBox.Grow(triangle.v0.x, triangle.v0.y, triangle.v0.z);
				rightBox.Grow(triangle.v1.x, triangle.v1.y, triangle.v1.z);
				rightBox.Grow(triangle.v2.x, triangle.v2.y, triangle.v2.z);
			}
			//   left AABB [num of triangle] * [surface area]
			// + right AABB [num of triangle] * [surface area]
			// -------------------------------------------------
			// = SAH cost
		}
		float cost = leftCount * leftBox.SurfaceArea() + rightCount * rightBox.SurfaceArea();
		return cost > 0 ? cost : std::numeric_limits<float>::max();
	}

	void __Subdivide_recur_old(unsigned int parentNodeIdx)
	{
		AABBNode& node = m_Nodes[parentNodeIdx];
		int bestAxis = -1;
		float bestPos = 0;
		float bestCost = std::numeric_limits<float>::max();
		for (int axis=0; axis < 3; ++axis) {
			for (size_t i=0; i < node.m_TriangeCount; i++) {
				auto triIdx = m_TriangleIndexBuffer[node.m_TriangleStartIndex + i];
				auto& triangle = m_Triangles[triIdx];
				// 노드 안의 모든 삼각형들을 돌면서, 각 삼각형의 무게중심을 기준으로 SAH 계산.
				const auto candidatePos = (triangle.v0[axis] + triangle.v1[axis] + triangle.v2[axis]) * 0.3333f;
				const float cost = ComputeCostbySAH_old(node, axis, candidatePos);
				if (cost < bestCost) { // find min cost
					bestPos = candidatePos;
					bestAxis = axis;
					bestCost = cost;
				}
			}
		}
		int axis = bestAxis; float splitPos = bestPos;

		const float parentArea = node.m_Bounds.SurfaceArea();
		const float parentCost = node.m_TriangeCount * parentArea;
		if (bestCost >= parentCost) return ;

		unsigned int startIdx = node.m_TriangleStartIndex;
		unsigned int endIdx = startIdx + node.m_TriangeCount - 1;
		while (startIdx <= endIdx) {
			const auto triIdx = m_TriangleIndexBuffer[startIdx];
			const auto centerOfTargetPrimitiveAxis = (m_Triangles[triIdx].v0[axis] + m_Triangles[triIdx].v1[axis] + m_Triangles[triIdx].v2[axis]) * 0.3333f;
			if (centerOfTargetPrimitiveAxis < splitPos) {
				startIdx++;
			} else {
				std::swap(m_TriangleIndexBuffer[startIdx], m_TriangleIndexBuffer[endIdx--]);
			}
		}

		// abort split if one of the sides is empty
		size_t leftCount = startIdx - node.m_TriangleStartIndex; // NOTE: ??
		if (leftCount == 0 || leftCount == node.m_TriangeCount) return; // NOTE: ??

		unsigned int leftChildIdx = m_Nodes.size();  // if 1
		node.m_Left = leftChildIdx;
		m_Nodes.emplace_back(node.m_TriangleStartIndex, leftCount);
		__UpdateNodeBounds(leftChildIdx);

		unsigned int rightChildIdx = m_Nodes.size(); // then 2
		node.m_Right = rightChildIdx;
		m_Nodes.emplace_back(startIdx, node.m_TriangeCount - leftCount);
		__UpdateNodeBounds(rightChildIdx);

		node.m_TriangeCount = 0;

		__Subdivide_recur_old(leftChildIdx);
		__Subdivide_recur_old(rightChildIdx);
	}

















public:
	void DebugRender(int bbox_show_level)
	{
		// 3일 경우, 3인 데이터
		for (auto &itr : m_AABBMeshList) {
			const auto bboxType = itr.second;
			if (bboxType.level >= 0 && bboxType.level <= bbox_show_level) { // if leaf
				itr.first->RenderMesh(GL_TRIANGLES);
			}
		}
	}

	// https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
	// build AABB tree with VAO & IBO array?
	// TODO: model을 추가할 때 마다 AABB가 자동으로 갱신.추가되도록 할 것.
	void AddPrimitive(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
	{
		// Pool of Triangles
		const size_t prevNumOfTriangles = m_Triangles.size();
		const size_t newNumOfTriangles = indices.size() / 3; // testObj's numOfTriangles == 4
		m_TotalTriangleCount = prevNumOfTriangles + newNumOfTriangles;
		m_Triangles.reserve(m_TotalTriangleCount); // 기존 크기 + 새로운 크기

		 // Triangles index buffer (for Pool)
		m_TriangleIndexBuffer.reserve(prevNumOfTriangles + newNumOfTriangles);

		for (int i = 0; i < newNumOfTriangles; i++) // 모든 triangle들이 정렬되어 있는 상태서 시작.
			m_TriangleIndexBuffer.push_back(i);

		// Pool of AABBTree node
		// Full Binary Tree 의 max node 개수는 2n-1 개이다. (n = 삼각형 개수)
		// 왜냐면, 트리의 leaf들에 모든 triangle들이 위치하기 때문에, 최대 분할 결과를 바탕으로 예측 가능하다.
		// leaf마다 triangle 1개 = max의 경우
		const size_t maxNumOfNodes = m_TotalTriangleCount * 2 - 1;
		m_Nodes.reserve(maxNumOfNodes);

		// NOTE: 최적화 필요. 일단 triangle array로 변환해서 멤버로 갖고 있지만, 이 과정이 필요가 없다고 보임.
		const int STRIDE = 8;
		for (size_t i = 0; i < newNumOfTriangles; ++i)
		{
			triangle_type primitive;
			for (int j = 0; j < 3; ++j)
			{   // for each vertex
				primitive[j] = { // Vertex Position     // Texture Coord // Normal Coord --> not used
						vertices[indices[i * 3 + j] * STRIDE + 0], vertices[indices[i * 3 + j] * STRIDE + 1], vertices[indices[i * 3 + j] * STRIDE + 2],
						vertices[indices[i * 3 + j] * STRIDE + 3], vertices[indices[i * 3 + j] * STRIDE + 4],
						vertices[indices[i * 3 + j] * STRIDE + 5], vertices[indices[i * 3 + j] * STRIDE + 6], vertices[indices[i * 3 + j] * STRIDE + 7],
				};
			}
			m_Triangles.emplace_back(primitive);
		}
		__BuildBVH_TopDown();
	}


	// NOTE: abb를 모델 매트릭스를 곱해서 덮어쓰는게 아니라, 충돌검사 직전 혹은 그리기 직전에 곱해서 그 임시 값으로 검사하는거다.
	AABBTree(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
	{
		AddPrimitive(vertices, indices);
	}

	// NOTE: 일단 복사 생성자 금지. [AABBTree t1 = AABB(...)]
	//		  추후에 이 부분 검토할 것.
	AABBTree& operator= (const AABBTree& other) = delete;

	~AABBTree()
	{

	}

	// Returns whether the AABB contains another AABB completely.
	// Because math is done to compute the minimum and maximum coordinates of the AABBs, overflow is possible for extreme values.
	bool Contains(const AABBTree& other);

	//Returns whether a point in 3D space is contained by the AABB, or not.
	// Because math is done to compute the minimum and maximum coordinates of the AABB, overflow is possible for extreme values.
	bool Contains(const glm::vec3& float3);

	// Determines the squared distance from a point to the nearest point to it that is contained by an AABB.
	glm::vec3 DistanceSquared(const glm::vec3& float3);

	// Returns a string representation of the AABB.
	std::string ToString();

	// Transforms an AABB by a 4x4 transformation matrix,
	// and returns a new AABB that contains the transformed AABB completely.
	AABBTree Transform(const glm::mat4x4 matrix);

	// NOTE: Moller-Trumbore fast ray-intersection test algorithm.
	// 		 paper ref: https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
	//		 use two affine triangle variable, U and V
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection.html
	// https://www.youtube.com/watch?v=EZXz-uPyCyA
	// https://www.youtube.com/watch?v=fK1RPmF_zjQ
	Hit IntersectTriangle_MollerTrumbore(const Ray& ray, const triangle_type& triangle)
	{
		const glm::vec3 v0 = triangle.v0.GetVertex();
		const glm::vec3 v1 = triangle.v1.GetVertex();
		const glm::vec3 v2 = triangle.v2.GetVertex();

		Hit hit; // default hit = no hit

		// find vectors for two edges sharing v0
		const glm::vec3 edge1 = v1 - v0;
		const glm::vec3 edge2 = v2 - v0;

		// begin calculating determinant - also used to calculate U parameter
		// this is need to solve with Kramer's rule.
		// reference: https://www.youtube.com/watch?v=jBsC34PxzoM
		const auto pVec = glm::cross(ray.direction, edge2);

		// if determinant is near zero, ray lies in plane of triangle
		const auto det = glm::dot(edge1, pVec);

		// if negative, then cull back face.
		if (det < 0.00001f) { // Epsilon
			return hit; // no hit
		}

		// calculate distance from v0 to ray origin.
		const auto tVec = ray.origin - v0;

		// calculate U parameter and test bounds.
		float U = glm::dot(tVec, pVec);
		if (U < 0.0f || U > det) {
			return hit; // no hit
		}

		// prepare to test V parameter
		const auto qVec = glm::cross(tVec, edge1);

		// calculate V parameter and test bounds.
		float V = glm::dot(ray.direction, qVec);
		if (V < 0.0f || U + V > det) {
			return hit; // no hit
		}

		// calculate t, scale parameters, ray intersects triangle
		float T = glm::dot(edge2, qVec);
		const float invert_det = 1.0 / det;
		T *= invert_det;
		U *= invert_det;
		V *= invert_det;

		// all done. calcaulte return values
		const auto faceNormal = glm::cross(v2 - v1, v0 - v1);
		const glm::vec3 hitPoint = v0 + glm::normalize(edge1) * U + glm::normalize(edge2) * V;

		const float K = 1.0f - U - V;
		hit.blendedPointNormal = triangle.v0.GetNormal() * K + triangle.v1.GetNormal() * U + triangle.v2.GetNormal() * V;
//		hit.blendedPointNormal = faceNormal; // flat shading

		hit.distance = T;
		hit.point = hitPoint;
		hit.faceNormal = glm::normalize(faceNormal);
		hit.triangle = triangle;
		return hit;
	}

	// NOTE: if not hit, then distance is -1
	// 홍정모 강의 버전 Triangle Intersection
	// compare 3 parts of sub-triangle.
	Hit IntersectTriangle_Hong(const Ray& ray, const triangle_type& triangle)
	{
		const glm::vec3 v0 = triangle.v0.GetVertex();
		const glm::vec3 v1 = triangle.v1.GetVertex();
		const glm::vec3 v2 = triangle.v2.GetVertex();

		Hit hit; // default hit = no hit
		const auto faceNormal = glm::cross(v2 - v1, v0 - v1);
		// (1) backface culling
		if (glm::dot(-ray.direction, faceNormal) < 0.0f)
			return hit;
		// (2) if ray is parallel to triangle
		if (glm::abs(glm::dot(ray.direction, faceNormal)) < 0.00001f)
			return hit;
		// (3) calcaute t (무한 평면)
		float t = (glm::dot(v1, faceNormal) - glm::dot(ray.origin, faceNormal)) / glm::dot(ray.direction, faceNormal);
		// (4) t가 음수. 즉 뒷방향
		if (t < 0.0f) return hit; // no hit
		// (5) 작은 삼각형 normal 계산 // 츙돌 점 p가 삼각형 안에 있는가?
		const glm::vec3 point = ray.origin + (t * ray.direction);

		const glm::vec3 n0 = (glm::cross(point - v2, v1 - v2));
		if (glm::dot(n0, faceNormal) < 0.0f) return hit; // no hit
		const glm::vec3 n1 = (glm::cross(point - v0, v2 - v0));
		if (glm::dot(n1, faceNormal) < 0.0f) return hit; // no hit
		const glm::vec3 n2 = (glm::cross(v1 - v0, point - v0));
		if (glm::dot(n2, faceNormal) < 0.0f) return hit; // no hit

		// (6) for Barycentric interpolation
		float area0 = glm::length(n0) * 0.5f;
		float area1 = glm::length(n1) * 0.5f;
		float area2 = glm::length(n2) * 0.5f;
		float areaSum = area0 + area1 + area2;
		float w0 = area0 / areaSum;
		float w1 = area1 / areaSum;
		float w2 = 1 - w0 - w1;

		// (7) TODO: normal blending 정보도 필요함. hit에 barycentric coord를 담기보단, 직접 blending한 값을 넣는것도 좋을 듯.
		hit.blendedPointNormal = triangle.v0.GetNormal() * w0 + triangle.v1.GetNormal() * w1 + triangle.v2.GetNormal() * w2;
		hit.distance = t;
		hit.point = point;
		hit.faceNormal = glm::normalize(faceNormal);
		hit.triangle = triangle;
		return hit;
	}

	// calculate ray-intersection.
	// NOTE: 방문 순서를 고정된 순서가 아닌, 자식 2개중 가까운 bounding를 우선 방문하는 것으로 결정. (BFS)
	// triangleIntersectMode 0 = default
	// triangleIntersectMode 0 = moller-trumbore
	Hit IntersectBVH(const Ray& ray, int triangleIntersectMode = 0)
	{
		Hit hit;
		if (m_Nodes[0].m_Bounds.Intersect(ray) < 0.0f) return hit; // no hit, early return.

		std::stack<uint> Stack;
		Stack.push(0);
		while (!Stack.empty())
		{
			const uint currIdx = Stack.top(); Stack.pop();
			AABBNode* node = &m_Nodes[currIdx]; // 시작 노드 (Root)에서 부터 접근.
			// if visited node is leaf, them check triangle intersection
			if (node->IsLeaf())
			{
				for (size_t i=0; i<node->m_TriangeCount; ++i)
				{
					// 여기서 삼각형 충돌을 감지했다 하더라도, 일단 모든 bvh를 돌아야 한다.
					const size_t triangleIndex = m_TriangleIndexBuffer[node->m_TriangleStartIndex + i];
					Hit new_hit;
					if (triangleIntersectMode == 0) {
						new_hit = IntersectTriangle_Hong(ray, m_Triangles[triangleIndex]);
					} else {
						new_hit = IntersectTriangle_MollerTrumbore(ray, m_Triangles[triangleIndex]);
					}
					if (new_hit.distance >= 0.0f) {
						hit = new_hit; // update hit
						break;
					}
				}
			}
			else
			{
				float distLeft = m_Nodes[node->m_Left].m_Bounds.Intersect(ray); // distance to left bbox
				float distRight = m_Nodes[node->m_Right].m_Bounds.Intersect(ray); // distance to right bbox
				if (distLeft >= 0.0f && distRight >= 0.0f) // visit close BBox first
				{
					if (distLeft > distRight)	{ Stack.push(node->m_Right); Stack.push(node->m_Left); } // visit right first
					else /* Left < Right */		{ Stack.push(node->m_Left); Stack.push(node->m_Right); } // visit left first
				}
				else if (distLeft >= 0.0f)
					Stack.push(node->m_Left);
				else if (distRight >= 0.0f)
					Stack.push(node->m_Right);
			}
		}
		return hit;
	}

	// HELPER FUNCTIONS
private:
	// Tree를 순회하면서 각 box에 대한 mesh를 생성, m_meshList에 삽입.
	// TODO: meshList가 배열이기에, 이걸 level에 따라 순회하려면 재귀를 BFS로 해줘야 한다.
	// 	     일단은 DFS로 함. 나중에 수정할 예정.

	struct GenMeshFormat {
		unsigned int idx = 0;
		int depth = 0;
	};

	// NOTE: OpenGL 함수는 쓰레딩을 조심해야 하네...;;;
	// 일단 OpenGL 함수 사용에 있어서 쓰레딩 구조는 빼자
	// ------------------------------------------------------------------
	// NOTE: https://stackoverflow.com/questions/11097170/multithreaded-rendering-on-opengl
	// NOTE: https://www.equalizergraphics.com/documentation/parallelOpenGLFAQ.html
	void __GenerateDebugMeshParallel(unsigned int node_idx)
	{
		Lunar::Timer timer;
		LOG_INFO("Generating Threaded BVH DebugMesh...  [total {1} node, {2} triangles]", node_idx, m_NodeCount, m_TotalTriangleCount);

		std::vector<std::future<void>> futures;
		futures.reserve(m_Nodes.size());

		// clear threads
//		m_ThreadPool.Shutdown(); NOTE: Shutdown 이상한 오류 발생.

		m_AABBMeshList.resize(m_Nodes.size());
		std::queue<GenMeshFormat> Queue;
		Queue.push({node_idx, 0});

		auto GenDebugMesh = [this](unsigned int idx, int depth) -> void
		{
		  AABBNode& currNode = m_Nodes[idx];

//		  LOG_TRACE("Thread {0} is subdividing node {1} -> depth: {2}", std::this_thread::get_id(), idx, depth);

		  auto bbox = currNode.m_Bounds;
		  const auto ub = bbox.m_UpperBound;
		  const auto lb = bbox.m_LowerBound;
		  float cube_vertices[] = { // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_05#Adding_the_3rd_dimension
				  /* front */ lb.x, lb.y,  ub.z, ub.x, lb.y,  ub.z, ub.x,  ub.y,  ub.z, lb.x,  ub.y,  ub.z,
				  /* back */ lb.x, lb.y, lb.z, ub.x, lb.y, lb.z, ub.x,  ub.y, lb.z, lb.x,  ub.y, lb.z
		  };
		  unsigned int cube_elements[] = {
				  /* front */ 0, 1, 2, 2, 3, 0,
				  /* right */ 1, 5, 6, 6, 2, 1,
				  /* back */ 7, 6, 5, 5, 4, 7,
				  /* left */ 4, 0, 3, 3, 7, 4,
				  /* bottom */4, 5, 1, 1, 0, 4,
				  /* top */ 3, 2, 6, 6, 7, 3
		  };

		  auto mesh_ptr = std::make_shared<AABB::Mesh>();

		  mesh_ptr->CreateMesh(cube_vertices, cube_elements, 24, 36);

		  auto p = currNode.IsLeaf();
		  auto k = p;

		  if (currNode.IsLeaf()) {
			  m_AABBMeshList[idx] = { mesh_ptr, bbox_level_type{depth, true} };
		  } else {
			  m_AABBMeshList[idx] = { mesh_ptr, bbox_level_type{depth, false} };
		  }

		  {
			  // NOTE: 별도 mutex 생성 없이 이 뮤텍스를 재사용하였음.
			  std::lock_guard<std::mutex> lock(m_TaskMutex);
			  m_MaxDepth = depth;
		  }
		};

		while (!Queue.empty())
		{
			GenMeshFormat& curr = Queue.front(); Queue.pop();
			AABBNode& currNode = m_Nodes[curr.idx];
			futures.push_back(m_ThreadPool.AddTask(GenDebugMesh, curr.idx, curr.depth));
			if (!currNode.IsLeaf()) {
				Queue.push({currNode.m_Left, curr.depth + 1});
				Queue.push({currNode.m_Right, curr.depth + 1});
			}
		}

		std::for_each(futures.begin(), futures.end(), [](std::future<void> &future) -> void
		{
		  future.wait();
		});
		LOG_INFO("BVH Threaded DebugMesh build finished at {0} ms", timer.ElapsedMillis());
	}

	// Change to Queue BFS
	void __GenerateDebugMeshBFS(unsigned int node_idx)
	{
		Lunar::Timer timer;
		LOG_INFO("Generating BVH DebugMesh...  [total {1} node, {2} triangles]", node_idx, m_NodeCount, m_TotalTriangleCount);

		// TODO: change to reserve
		m_AABBMeshList.resize(m_Nodes.size());
		//    	   idx           depth
		std::queue<GenMeshFormat> Queue;
		Queue.push({node_idx, 0});

		while (!Queue.empty())
		{
			GenMeshFormat& curr = Queue.front(); Queue.pop();
			AABBNode& currNode = m_Nodes[curr.idx];

			// 방문
			auto bbox = currNode.m_Bounds;
			const auto ub = bbox.m_UpperBound;
			const auto lb = bbox.m_LowerBound;
			float cube_vertices[] = { // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_05#Adding_the_3rd_dimension
									 /* front */ lb.x, lb.y,  ub.z, ub.x, lb.y,  ub.z, ub.x,  ub.y,  ub.z, lb.x,  ub.y,  ub.z,
									 /* back */ lb.x, lb.y, lb.z, ub.x, lb.y, lb.z, ub.x,  ub.y, lb.z, lb.x,  ub.y, lb.z
			};
			unsigned int cube_elements[] = {
					/* front */ 0, 1, 2, 2, 3, 0,
					/* right */ 1, 5, 6, 6, 2, 1,
					/* back */ 7, 6, 5, 5, 4, 7,
					/* left */ 4, 0, 3, 3, 7, 4,
					/* bottom */4, 5, 1, 1, 0, 4,
					/* top */ 3, 2, 6, 6, 7, 3
			};

			auto mesh_ptr = std::make_shared<AABB::Mesh>();
			mesh_ptr->CreateMesh(cube_vertices, cube_elements, 24, 36);

			if (currNode.IsLeaf()) {
				m_AABBMeshList[curr.idx] = { mesh_ptr, bbox_level_type{curr.depth, true} };
			} else {
				m_AABBMeshList[curr.idx] = { mesh_ptr, bbox_level_type{curr.depth, false} };
			}
			 m_MaxDepth = curr.depth;

			if (!currNode.IsLeaf()) {
				Queue.push({currNode.m_Left, curr.depth + 1});
				Queue.push({currNode.m_Right, curr.depth + 1});
			}
		}
		LOG_INFO("BVH DebugMesh build finished at {0} ms", timer.ElapsedMillis());
	}
};

#endif //SCOOP_AABB_H

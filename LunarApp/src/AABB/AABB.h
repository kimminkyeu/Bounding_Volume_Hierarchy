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

#include "LunarApp/src/AABB/Mesh.h"

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
};

struct Hit // hit point
{
	glm::vec3 postion;
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
	float SurfaceArea()
	{
		glm::vec3 d = m_UpperBound - m_LowerBound;
		return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
	}

	// box를 주어진 vertex를 포함하도록 키움.
	void Grow(const glm::vec3& v)
	{
		m_LowerBound = glm::vec3(std::min(m_LowerBound.x, v.x), std::min(m_LowerBound.y, v.y), std::min(m_LowerBound.z, v.z));
		m_UpperBound = glm::vec3(std::max(m_UpperBound.x, v.x), std::max(m_UpperBound.y, v.y), std::max(m_UpperBound.z, v.z));
	}

	void Grow(float x, float y, float z)
	{
		m_LowerBound = glm::vec3(std::min(m_LowerBound.x, x), std::min(m_LowerBound.y, y), std::min(m_LowerBound.z, z));
		m_UpperBound = glm::vec3(std::max(m_UpperBound.x, x), std::max(m_UpperBound.y, y), std::max(m_UpperBound.z, z));
	}

	// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html
	// https://youtu.be/TrqK-atFfWY?t=1349
	bool Intersect(const Ray& ray) // hitPoint = rayOrigin + rayDirection * t;  --> for every x, y, z plain, t must be the same.
	{
		// get t for x axis
		float tx1 = (m_LowerBound.x - ray.origin.x) / ray.direction.x;
		float tx2 = (m_UpperBound.x - ray.origin.x) / ray.direction.x;
		// get t for y axis
		float ty1 = (m_LowerBound.y - ray.origin.y) / ray.direction.y;
		float ty2 = (m_UpperBound.y - ray.origin.y) / ray.direction.y;
		// get t for z axis
		float tz1 = (m_LowerBound.z - ray.origin.z) / ray.direction.z;
		float tz2 = (m_UpperBound.z - ray.origin.z) / ray.direction.z;
		// get min, max for every t
		float txmin = glm::min(tx1, tx2); float txmax = glm::max(tx1, tx2);
		float tymin = glm::min(ty1, ty2); float tymax = glm::max(ty1, ty2);
		float tzmin = glm::min(tz1, tz2); float tzmax = glm::max(tz1, tz2);
		// get tmin, tmax
		float tmin = glm::max(txmin, tymin); tmin = glm::max(tmin, tzmin);
		float tmax = glm::min(txmax, tymax); tmax = glm::min(tmax, tzmax);

		/*
		 * TODO: Implement intersection pass logic
		 * https://gdbooks.gitbooks.io/3dcollisions/content/Chapter3/raycast_aabb.html
		*/
		// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
		if (tmax < 0.0f) {
			return false;
		}

		// if tmin > tmax, ray doesn't intersect AABB
		if (tmin > tmax) {
			return false;
		}

		return true;
	}
};


// NOTE: Input Data is an array of triangles.
//union TriangleDataBaseFormat// VBO structure -> x, y, z, tx, ty, nx, ny, nz
//{
//	struct {
//		float x;
//		float y;
//		float z;
//		float tx;
//		float ty;
//		float nx;
//		float ny;
//		float nz;
//	};
//	float d[8];
//};

template <typename T>
struct Triangle // Triangle[0] = Triangle.v0
{
	T v0;
	T v1;
	T v2;

	Triangle() = default;
	Triangle(T _v0, T _v1, T _v2) noexcept
		: v0(_v0), v1(_v1), v2(_v2)
	{}

	T& operator[] (int index)
	{
		if (index >= 3) throw std::out_of_range("index out of range");
		if (index == 0) return v0;
		if (index == 1) return v1;
		if (index == 2) return v2;
	}

	const T& operator[] (int index) const
	{
		if (index >= 3) throw std::out_of_range("index out of range");
		if (index == 0) return v0;
		if (index == 1) return v1;
		if (index == 2) return v2;
	}
};


// ------------------------------------------------
// Implementation of Axis-aligned bounding box
struct AABBNode
{
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
	using triangle_type = Triangle<glm::vec3>;
	// ...
private: // member data
	std::vector<AABBNode> m_Nodes; // Node Pool
	unsigned int m_RootIndex = 0; // root index of node pool
	std::vector<triangle_type> m_Triangles; // primitive pool
	std::vector<size_t> m_TriangleIndexBuffer; // just like IBO, we change triangle sequence with this.

private: // member data tmp
	using bbox_level_type = unsigned int; // tree depth of bbox
	using aabb_mesh_data_type = std::pair<std::shared_ptr<AABB::Mesh>, bbox_level_type>;
	std::vector<aabb_mesh_data_type> m_AABBMeshList;

private:
	// Subdivide space
	void __BuildBVH_TopDown()
	{
		m_Nodes.clear();
		// to start, assign all triangles to root node.
		m_Nodes.emplace_back( 0, m_Triangles.size() ); // (0) insert node at root
		__UpdateNodeBounds(m_RootIndex); // (1) Update Each Bode Bound
		__Subdivide_recur(m_RootIndex); // (2) Subdivide space recursively
//		__PrintTreeDebug();
		__GenerateDebugMesh_recur(m_RootIndex, 0); // (3) for debug render, generate mesh(VAO, VBO.. etc) for each Bounding Box;
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

	// Surface Area Huristic
	float ComputeCostbySAH(AABBNode& node, int axis, float splitPos)
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

	void __Subdivide_recur(unsigned int parentNodeIdx)
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
				const float cost = ComputeCostbySAH(node, axis, candidatePos);
				if (cost < bestCost) { // find min cost
					bestPos = candidatePos;
					bestAxis = axis;
					bestCost = cost;
				}
			}
		}
		int axis = bestAxis;
		float splitPos = bestPos;

		// NOTE: recursion stops here -----------------------
		// Split은 자식 둘의 SAH합이 부모의 SAH보다 작을 경우에만 진행.
		const float parentArea = node.m_Bounds.SurfaceArea();
		const float parentCost = node.m_TriangeCount * parentArea;
		if (bestCost >= parentCost) return ;


		// *********************************
		// split via axis
		unsigned int startIdx = node.m_TriangleStartIndex;
		unsigned int endIdx = startIdx + node.m_TriangeCount - 1;
		while (startIdx <= endIdx)
		{
			// 해당 삼각형의 axis의 중심과 splitPos를 비교 // 삼각형 중심으로 분할하기 때문에, 겹치는 영역이 발생.
			const auto triIdx = m_TriangleIndexBuffer[startIdx];
			const auto centerOfTargetPrimitiveAxis = (m_Triangles[triIdx].v0[axis] + m_Triangles[triIdx].v1[axis] + m_Triangles[triIdx].v2[axis]) * 0.3333f;
			if (centerOfTargetPrimitiveAxis < splitPos) {
				startIdx++;
			} else {
				// same as quick-sort pivot move
				std::swap(m_TriangleIndexBuffer[startIdx], m_TriangleIndexBuffer[endIdx--]);
			}
		}

		// abort split if one of the sides is empty
		size_t leftCount = startIdx - node.m_TriangleStartIndex; // NOTE: ??
		if (leftCount == 0 || leftCount == node.m_TriangeCount) return; // NOTE: ??

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

		// recurse
		__Subdivide_recur(leftChildIdx);
		__Subdivide_recur(rightChildIdx);
	}


public:
	void DebugRender(int bbox_level)
	{
		for (auto &itr : m_AABBMeshList) {
			if (itr.second == bbox_level) { // 특정 bbox level만 그리기 위함.
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
		m_Triangles.reserve(prevNumOfTriangles + newNumOfTriangles); // 기존 크기 + 새로운 크기

		// Triangles index buffer (for Pool)
		m_TriangleIndexBuffer.reserve(prevNumOfTriangles + newNumOfTriangles);

		for (int i = 0; i < newNumOfTriangles; i++) // 모든 triangle들이 정렬되어 있는 상태서 시작.
			m_TriangleIndexBuffer.push_back(i);

		// Pool of AABBTree node
		// Full Binary Tree 의 max node 개수는 2n-1 개이다.
		const size_t maxNumOfNodes = newNumOfTriangles * 2 - 1;
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
//						vertices[indices[i * 3 + j] * STRIDE + 3], vertices[indices[i * 3 + j] * STRIDE + 4],
//						vertices[indices[i * 3 + j] * STRIDE + 5], vertices[indices[i * 3 + j] * STRIDE + 6], vertices[indices[i * 3 + j] * STRIDE + 7],
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

	~AABBTree();

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

	// NOTE: if not hit, then distance is -1
	struct Hit
	{
		float distance = -1.0f;
		glm::vec3 normal = glm::vec3(0.0f);
		glm::vec3 point = glm::vec3(0.0f);
	};

	Hit IntersectTriangle(const Ray& ray, const triangle_type& triangle)
	{
		Hit hit; // default hit = no hit
		const auto triangleNormal = glm::cross(triangle.v2 - triangle.v1, triangle.v0 - triangle.v1);
		// (1) backface culling
		if (glm::dot(-ray.direction, triangleNormal) < 0.0f)
			return hit;
		// (2) if ray is parallel to triangle
		if (glm::abs(glm::dot(ray.direction, triangleNormal)) < 0.00001f)
			return hit;
		// (3) calcaute t (무한 평면)
		float t = (glm::dot(triangle.v1, triangleNormal) - glm::dot(ray.origin, triangleNormal)) / glm::dot(ray.direction, triangleNormal);
		// (4) t가 음수. 즉 뒷방향
		if (t < 0.0f) return hit; // no hit
		// (5) 작은 삼각형 normal 계산 // 츙돌 점 p가 삼각형 안에 있는가?
		const glm::vec3 point = ray.origin + (t * ray.direction);
		const glm::vec3 n0 = (glm::cross(point - triangle.v2, triangle.v1 - triangle.v2));
		if (glm::dot(n0, triangleNormal) < 0.0f) return hit; // no hit
		const glm::vec3 n1 = (glm::cross(point - triangle.v0, triangle.v2 - triangle.v0));
		if (glm::dot(n1, triangleNormal) < 0.0f) return hit; // no hit
		const glm::vec3 n2 = (glm::cross(triangle.v1 - triangle.v0, point - triangle.v0));
		if (glm::dot(n2, triangleNormal) < 0.0f) return hit; // no hit
		// (6) TODO: texture 정보에 따라 추가 구현.

		// return valid hit result.
		hit.distance = t;
		hit.point = point;
		hit.normal = triangleNormal;
		return hit;
	}

	// calculate ray-intersection.
	Hit IntersectBVH(const Ray& ray, const uint nodeIdx = 0)
	{
		AABBNode& node = m_Nodes[nodeIdx]; // 시작 노드 (Root)에서 부터 접근.
		if (!node.m_Bounds.Intersect(ray))
		{
			return Hit(); // no hit
		}
		if (node.IsLeaf())
		{
			for (size_t i=0; i<node.m_TriangeCount; ++i) {
				const auto triangleIndex = m_TriangleIndexBuffer[node.m_TriangleStartIndex + i];
				// NOTE: Triangle 충돌 함수 검증 필요.
				Hit hit = IntersectTriangle(ray, m_Triangles[triangleIndex]);
				if (hit.distance > 0.0f) { // if hit success
					return hit;
				}
			}
			assert(false && "[IntersectBVH] Bounding Box is wrong...");
		}
		else // if intersected + not leaf
		{
			Hit h1 = IntersectBVH(ray, node.m_Left);
			Hit h2 = IntersectBVH(ray, node.m_Right);

			// NOTE: 이 부분 역시 검증 필요.
			// 둘다 충돌일 경우, 두 Hit 중에서 양수이면서 가까운 값을 최종 반환. (2개 bounding box 모두 충돌 가능하기 때문)
			if (h1.distance > 0.0f && h2.distance > 0.0f)
				return (h1.distance < h2.distance ? h1 : h2);
			else if (h1.distance > 0.0f)
				return h1;
			else
				return h2;
		}
	}

	// HELPER FUNCTIONS
private:
	// Tree를 순회하면서 각 box에 대한 mesh를 생성, m_meshList에 삽입.
	// TODO: meshList가 배열이기에, 이걸 level에 따라 순회하려면 재귀를 BFS로 해줘야 한다.
	// 	     일단은 DFS로 함. 나중에 수정할 예정.
	void __GenerateDebugMesh_recur(unsigned int node_idx, bbox_level_type depth)
	{
		auto i = m_Nodes[node_idx];

		auto bbox = m_Nodes[node_idx].m_Bounds;
		const auto ub = bbox.m_UpperBound;
		const auto lb = bbox.m_LowerBound;

		float cube_vertices[] = { // https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_05#Adding_the_3rd_dimension
								 // front
								 lb.x, lb.y,  ub.z,
								 ub.x, lb.y,  ub.z,
								 ub.x,  ub.y,  ub.z,
								 lb.x,  ub.y,  ub.z,
								 // back
								 lb.x, lb.y, lb.z,
								 ub.x, lb.y, lb.z,
								 ub.x,  ub.y, lb.z,
								 lb.x,  ub.y, lb.z
		};

		unsigned int cube_elements[] = {
				// front
				0, 1, 2,
				2, 3, 0,
				// right
				1, 5, 6,
				6, 2, 1,
				// back
				7, 6, 5,
				5, 4, 7,
				// left
				4, 0, 3,
				3, 7, 4,
				// bottom
				4, 5, 1,
				1, 0, 4,
				// top
				3, 2, 6,
				6, 7, 3
		};

		auto mesh_ptr = std::make_shared<AABB::Mesh>();
		mesh_ptr->CreateMesh(cube_vertices, cube_elements, 24, 36);
		m_AABBMeshList.emplace_back(mesh_ptr, depth);

		if (depth > m_MaxDepth) { // just for IMGUI debug draw.
			m_MaxDepth = depth;
		}

		// if not leaf, then traverse every tree to create mesh
		if (!m_Nodes[node_idx].IsLeaf())
		{
			__GenerateDebugMesh_recur(m_Nodes[node_idx].m_Left, depth + 1);
			__GenerateDebugMesh_recur(m_Nodes[node_idx].m_Right, depth + 1);
		}
	}

	void __PrintTreeDebug()
	{
		// traverse bfs. // store index of node.
		using node_index = int;
		using tree_level = int;
		std::queue<std::pair<node_index, tree_level>> queue;
		queue.push({m_RootIndex, 0});
		int prev_level = -1;
		while (!queue.empty())
		{
			auto top = queue.front(); queue.pop();
			auto node = m_Nodes[top.first];
			queue.push({node.m_Left, top.second + 1});
			queue.push({node.m_Right, top.second + 1});

			if (prev_level != top.second)
				std::cout << "\n" << "L" << top.second << "      ";
			if (node.IsLeaf()) {
				std::cout << std::left << std::setw(5) << "#" << " ";
			} else {
				std::cout << std::left << std::setw(5) << top.first << " ";
			}
			prev_level = top.second;
		}
		std::cout << "\n";
	}
};

#endif //SCOOP_AABB_H

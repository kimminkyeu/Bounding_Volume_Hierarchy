//
// Created by Minkyeu Kim on 8/20/23.
//

#ifndef SCOOP_AABB_H
#define SCOOP_AABB_H


#include <glm.hpp>
#include <string>
//#include <math.h> // fminf
#include <vector>
#include <stdexcept>

#include "LunarApp/src/AABB/Mesh.h"

// TODO: 구현이 끝나면, Template으로 바꿀 수 있는 부분 체크하기. (glm::vec3, glm::vec2, custom vec2 ... etc)

// ********************************************************************
// NOTE: https://jacco.ompf2.com/  --> 이 사람 내용을 보면서 공부할 것!
// ***********************************************************************
// https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf --> 이건 참조용.
// https://mshgrid.com/2021/01/17/aabb-tree/ --> 이것도 참조용.
// ********************************************************************


// AABB는 normal과 texture가 필요 없기 때문에, 아래와 같이 별도로 상속하여 오버라이딩함.
/*
class AABBDebugMesh : public Lunar::Mesh
{
public:
	AABBDebugMesh()
			: AABBDebugMesh::Mesh()
	{};

//	~AABBDebugMesh();
	void CreateMesh(GLfloat* verticies, unsigned int *indicies, unsigned int numOfVertices, unsigned int numOfIndicies) override
	{
		// VAO (Vertex Array)
		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		// IBO (Index Buffer)
		glGenBuffers(1, &m_IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies[0]) * numOfIndicies, indicies, GL_STATIC_DRAW);

		// VBO (Vertex Buffer)
		glGenBuffers(1, &m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticies[0]) * numOfVertices, verticies, GL_STATIC_DRAW);

		// Set vertex attribute : [ x y z U V ]
		// x y z (stride = X ~ next X 까지의 거리)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(0);

		// Unbind Buffer for later use
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
};
 */

// https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf
// Given two bounding boxes we can compute the union with min and max operations.
// These can be made efficient using SIMD.
struct BoundingBox
{
	glm::vec3 m_LowerBound = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 m_UpperBound = glm::vec3(std::numeric_limits<float>::min());

	// compute the surface area of bounding box (표면적)
	float SurfaceArea()
	{
		glm::vec3 d = m_UpperBound - m_LowerBound;
		return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
	}
};


// NOTE: Input Data is an array of triangles.
union TriangleDataBaseFormat// VBO structure -> x, y, z, tx, ty, nx, ny, nz
{
	struct {
		float x;
		float y;
		float z;
		float tx;
		float ty;
		float nx;
		float ny;
		float nz;
	};
	float d[8];
};

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
		return index == 0 ? v0 : (index == 1 ? v1 : v2);
	}

	const T& operator[] (int index) const
	{
		if (index >= 3) throw std::out_of_range("index out of range");
		return index == 0 ? v0 : (index == 1 ? v1 : v2);
	}
};


// ------------------------------------------------
// Implementation of Axis-aligned bounding box
struct AABBNode
{
	AABBNode(unsigned int primitiveStartIdx, size_t primitiveSize)
		: m_PrimitiveStartIndex(primitiveStartIdx), m_PrimitiveSize(primitiveSize)
	{};

	BoundingBox m_Bounds; // bounding box of the node
	unsigned int m_Left = -1;
	unsigned int m_Right = -1;
	unsigned int m_PrimitiveStartIndex = 0; // store the index of the first primitive, and the number of primitives.
	size_t m_PrimitiveSize = 0; // Node가 가질 primitive 길이.

	inline bool isLeaf() const noexcept
	{ return (m_PrimitiveSize > 0); }
};

// ------------------------------------------------
// Implementation of Axis-aligned bounding box
class AABBTree
{
public:
	unsigned int m_MaxDepth = 0; // 0 = root
private: // typedef and namespace scope
	using triangle_type = Triangle<TriangleDataBaseFormat>;
	// ...
private: // member data
	std::vector<AABBNode> m_Nodes; // Node Pool
	unsigned int m_RootIndex = 0; // root index of node pool
	std::vector<triangle_type> m_Primitives; // primitive pool
	std::vector<size_t> m_PrimitiveIndexBuffer; // just like IBO, we change triangle sequence with this.
	// NOTE: 중요! m_Primitive를 직접 바꾸는게 아니라, 그 index를 바꾸는 거다.

private: // member data tmp
	using bbox_level_type = unsigned int; // tree depth of bbox
	using data_type = std::pair<std::shared_ptr<AABB::Mesh>, bbox_level_type>;
	std::vector<data_type> m_AABBMeshList;

private:
	// Subdivide space
	void __BuildBVH()
	{
		// to start, assign all triangles to root node.
		m_Nodes.emplace_back( 0, m_Primitives.size() ); // (0) insert node at root
		__UpdateNodeBounds(m_RootIndex); // (1) Update Each Bode Bound
		__Subdivide_recur(m_RootIndex); // (2) Subdivide space recursively
		__GenerateDebugMesh_recur(m_RootIndex, 0); // (3) for debug render, generate mesh(VAO, VBO.. etc) for each Bounding Box;
	}

	void __UpdateNodeBounds(unsigned int nodeIdx)
	{
		AABBNode* targetNode = &(m_Nodes[nodeIdx]);
		BoundingBox* bbox = &(targetNode->m_Bounds);
		const unsigned int start = targetNode->m_PrimitiveStartIndex;
		// update bounding box for node's every primitive
		for (unsigned int i = 0; i < targetNode->m_PrimitiveSize; ++i) {
			const auto triIdx = m_PrimitiveIndexBuffer[start + i];
			for (int v=0; v<3; ++v) {
				// 모든 삼각형의 모든 vertex를 돌면서, vec3 bound와 vec3 v0 중 최소값을 bound에 갱신.
				const auto tri = (m_Primitives[triIdx])[v];
				bbox->m_LowerBound = glm::vec3(std::min(bbox->m_LowerBound.x, tri.x), std::min(bbox->m_LowerBound.y, tri.y), std::min(bbox->m_LowerBound.z, tri.z));
				bbox->m_UpperBound = glm::vec3(std::max(bbox->m_UpperBound.x, tri.x), std::max(bbox->m_UpperBound.y, tri.y), std::max(bbox->m_UpperBound.z, tri.z));
			}
		}
	}

	void __Subdivide_recur(unsigned int parentNodeIdx)
	{
		AABBNode& node = m_Nodes[parentNodeIdx];

		// terminate recursion
		if (node.m_PrimitiveSize <= 2) return ; // min is Mesh of 2 triangles

		// determine split axis and position (AABB)
		const auto bbox = node.m_Bounds;
		const auto d = bbox.m_UpperBound - bbox.m_LowerBound;
		int axis = 0; // x
		if (d.y > d.x) axis = 1; // y
		if (d.z > d[axis]) axis = 2; // z
		float splitPos = bbox.m_LowerBound[axis] + d[axis] * 0.5f;

		// split axis in half
//		auto k = m_PrimitiveIndexBuffer;
		unsigned int startIdx = node.m_PrimitiveStartIndex;
		unsigned int endIdx = startIdx + node.m_PrimitiveSize - 1; // = past end.
		while (startIdx <= endIdx)
		{
			// 해당 삼각형의 axis의 중심과 splitPos를 비교
			// 삼각형 중심으로 분할하기 때문에, 겹치는 영역이 발생.
			const auto triIdx = m_PrimitiveIndexBuffer[startIdx];
			const auto centerOfTargetPrimitiveAxis = (m_Primitives[triIdx].v0.d[axis] + m_Primitives[triIdx].v1.d[axis] + m_Primitives[triIdx].v2.d[axis]) * 0.3333f;
			if (centerOfTargetPrimitiveAxis < splitPos) {
				startIdx++;
			} else {
				// 그냥 삼각형들을 오른족으로 하나씩 Swap. (정렬 x)
				std::swap(m_PrimitiveIndexBuffer[startIdx], m_PrimitiveIndexBuffer[endIdx--]);
			}
		}
		// NOTE: half로 자르면 빈 bbox가 생길수 있나? 없지 않나...?
		// abort split if one of the sides is empty
		size_t leftCount = startIdx - node.m_PrimitiveStartIndex; // NOTE: ??
		if (leftCount == 0 || leftCount == node.m_PrimitiveSize) return; // NOTE: ??

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
		m_Nodes.emplace_back(node.m_PrimitiveStartIndex, leftCount);

		// insert right
		unsigned int rightChildIdx = m_Nodes.size(); // then 2
		node.m_Right = rightChildIdx;
		m_Nodes.emplace_back(startIdx, node.m_PrimitiveSize - leftCount);

		// set prim count to 0, because it's not a leaf node anymore.
		node.m_PrimitiveSize = 0;

		__UpdateNodeBounds(leftChildIdx);
		__UpdateNodeBounds(rightChildIdx);

		// recurse
		__Subdivide_recur(leftChildIdx);
		__Subdivide_recur(rightChildIdx);
	}

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
		if (!m_Nodes[node_idx].isLeaf())
		{
			__GenerateDebugMesh_recur(m_Nodes[node_idx].m_Left, depth + 1);
			__GenerateDebugMesh_recur(m_Nodes[node_idx].m_Right, depth + 1);
		}
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


	// NOTE: abb를 모델 매트릭스를 곱해서 덮어쓰는게 아니라, 충돌검사 직전 혹은 그리기 직전에 곱해서 그 임시 값으로 검사하는거다.
	AABBTree(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
	{
		// Pool of Triangles
		const size_t numOfTriangles = indices.size() / 3; // testObj's numOfTriangles == 4
		m_Primitives.reserve(numOfTriangles);

		// Triangles index buffer (for Pool)
		m_PrimitiveIndexBuffer.reserve(numOfTriangles);

		for (int i = 0; i < numOfTriangles; i++) // 모든 triangle들이 정렬되어 있는 상태서 시작.
			m_PrimitiveIndexBuffer.push_back(i);

		// Pool of AABBTree node
		// Full Binary Tree 의 max node 개수는 2n-1 개이다.
		const size_t maxNumOfNodes = numOfTriangles * 2 - 1;
		m_Nodes.reserve(maxNumOfNodes);

		// NOTE: 최적화 필요. 일단 triangle array로 변환해서 멤버로 갖고 있지만, 이 과정이 필요가 없다고 보임.
		const int STRIDE = 8;
		for (size_t i = 0; i < numOfTriangles; ++i)
		{
			triangle_type primitive;
			for (int j = 0; j < 3; ++j)
			{   // for each vertex
				primitive[j] = { // Vertex Position // Texture Coord // Normal Coord
						vertices[indices[i * 3 + j] * STRIDE + 0], vertices[indices[i * 3 + j] * STRIDE + 1], vertices[indices[i * 3 + j] * STRIDE + 2],
						vertices[indices[i * 3 + j] * STRIDE + 3], vertices[indices[i * 3 + j] * STRIDE + 4],
						vertices[indices[i * 3 + j] * STRIDE + 5], vertices[indices[i * 3 + j] * STRIDE + 6], vertices[indices[i * 3 + j] * STRIDE + 7],
				};
			}
			m_Primitives.emplace_back(primitive);
		}
		__BuildBVH();
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

};

#endif //SCOOP_AABB_H

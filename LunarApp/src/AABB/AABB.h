//
// Created by Minkyeu Kim on 8/20/23.
//

#ifndef SCOOP_AABB_H
#define SCOOP_AABB_H


#include <glm.hpp>
#include <string>
#include <vector>

// ********************************************************************
// NOTE: https://jacco.ompf2.com/  --> 이 사람 내용을 보면서 공부할 것!
// ***********************************************************************
// https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf --> 이건 참조용.
// https://mshgrid.com/2021/01/17/aabb-tree/ --> 이것도 참조용.
// ********************************************************************

// https://box2d.org/files/ErinCatto_DynamicBVH_Full.pdf
// Given two bounding boxes we can compute the union with min and max operations.
// These can be made efficient using SIMD.
struct BoundingBox
{
	glm::vec3 m_LowerBound;
	glm::vec3 m_UpperBound;

	// compute the surface area of bounding box (표면적)
	float SurfaceArea()
	{
		glm::vec3 d = m_UpperBound - m_LowerBound;
		return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
	}
};

// 두 BoundingBox를 함치는 경우..
BoundingBox Union(BoundingBox A, BoundingBox B)
{
	BoundingBox C;
	C.m_LowerBound = glm::min(A.m_LowerBound, B.m_LowerBound);
	C.m_UpperBound = glm::min(A.m_UpperBound, B.m_UpperBound);
	return C;
}

// ------------------------------------------------
// Implementation of Axis-aligned bounding box
struct AABBNode
{
	BoundingBox m_Bounds; // bounding box of the node
	int m_Left, m_Right;
	bool m_isLeaf;
	int m_ObjectIndex; // store the index of the first primitive, and the number of primitives.
};

// ------------------------------------------------
// Implementation of Axis-aligned bounding box
class AABBTree
{
private:
	std::vector<AABBNode> m_Nodes; // Node Pool

public:
	// https://jacco.ompf2.com/2022/04/13/how-to-build-a-bvh-part-1-basics/
	// build AABB tree with VAO & IBO array
	AABBTree(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
	{
		const size_t numOfTriangles = indices.size() / 3;
		// Full Binary Tree 의 max node 개수는 2n-1 개이다.
		const size_t maxNumOfNodes = numOfTriangles * 2 - 1;
		m_Nodes.reserve(maxNumOfNodes);
		// ...
	}

	~AABBTree();

	// Returns whether the AABB contains another AABB completely.
	// Because math is done to compute the minimum and maximum coordinates of the AABBs, overflow is possible for extreme values.
	bool Contains(const AABBTree& other) {};

	//Returns whether a point in 3D space is contained by the AABB, or not.
	// Because math is done to compute the minimum and maximum coordinates of the AABB, overflow is possible for extreme values.
	bool Contains(const glm::vec3& float3) {};

	// Determines the squared distance from a point to the nearest point to it that is contained by an AABB.
	glm::vec3 DistanceSquared(const glm::vec3& float3) {};

	// Returns a string representation of the AABB.
	std::string ToString() {};

	// Transforms an AABB by a 4x4 transformation matrix,
	// and returns a new AABB that contains the transformed AABB completely.
	AABBTree Transform(const glm::mat4x4 matrix) {};
};

#endif //SCOOP_AABB_H

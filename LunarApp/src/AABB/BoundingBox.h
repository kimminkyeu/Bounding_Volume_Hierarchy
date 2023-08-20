//
// Created by Minkyeu Kim on 8/20/23.
//

#ifndef SCOOP_BOUNDINGBOX_H
#define SCOOP_BOUNDINGBOX_H

#include <glm.hpp>

// Unity Member : https://docs.unity3d.com/2022.3/Documentation/ScriptReference/Bounds.html
// center
// extents
// max
// min
// size

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

BoundingBox Union(BoundingBox A, BoundingBox B)
{
	BoundingBox C;
	C.m_LowerBound = glm::min(A.m_LowerBound, B.m_LowerBound);
	C.m_UpperBound = glm::min(A.m_UpperBound, B.m_UpperBound);
	return C;
}



#endif //SCOOP_BOUNDINGBOX_H

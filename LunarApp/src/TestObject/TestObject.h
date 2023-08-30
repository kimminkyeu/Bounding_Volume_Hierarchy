//
// Created by USER on 2023-08-23.
//

#ifndef SCOOP_TESTOBJECT_H
#define SCOOP_TESTOBJECT_H

#include <vector>
#include <glm.hpp>
#include <memory>
#include <Lunar/Mesh/Mesh.h>
#include <Lunar/Core/Log.h>

class TestObject {
private:
	std::vector<std::shared_ptr<Lunar::Mesh>> m_MeshList;
public:
//	std::vector<float> m_Vertices = {
//			-1.0f, -1.0f, -0.5f,	// v0. x y z
//			0.0f, -1.0f, 1.0f,
//			1.0f, -1.0f, -0.5f, 	// v1. x y z
//			0.0f, 1.0f, 0.0f 	// v2. x y z
//	};
//	std::vector<unsigned int> m_Indices = {
//			0, 1, 3,
//			1, 2, 3,
//			2, 0, 3,
//			0, 2, 1,
//	};
	std::vector<float> m_Vertices = {
			-1.0f, -1.0f, 0.0f,	// v0. x y z
			1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 	// v1. x y z
			1.0f, -1.0f, 0.0f 	// v2. x y z
	};
	std::vector<unsigned int> m_Indices = {
			0, 1, 2,
			0, 3, 1,
	};
public:
	// Test object for AABB
	TestObject()
	{
		std::vector<glm::vec3> normals;
		std::vector<float> vertices;
		for (int i = 0; i < m_Indices.size()/3; ++i)
		{
			// i = 0, 1
			const auto p0 = glm::vec3{m_Vertices[m_Indices[i * 3 + 0] * 3 + 0], m_Vertices[m_Indices[i * 3 + 0] * 3 + 1], m_Vertices[m_Indices[i * 3 + 0] * 3 + 2]};
			const auto p1 = glm::vec3{m_Vertices[m_Indices[i * 3 + 1] * 3 + 0], m_Vertices[m_Indices[i * 3 + 1] * 3 + 1], m_Vertices[m_Indices[i * 3 + 1] * 3 + 2]};
			const auto p2 = glm::vec3{m_Vertices[m_Indices[i * 3 + 2] * 3 + 0], m_Vertices[m_Indices[i * 3 + 2] * 3 + 1], m_Vertices[m_Indices[i * 3 + 2] * 3 + 2]};
			auto normal = GetNormal(p0, p1, p2);
			LOG_INFO("{0} {1} {2}", normal.x, normal.y, normal.z);
			normals.push_back(normal);
		}
		for (int i = 0; i < m_Vertices.size()/3; ++i) {
			vertices.insert(vertices.end(), {m_Vertices[i * 3 + 0], m_Vertices[i * 3 + 1], m_Vertices[i * 3 + 2]}); // Vertex
			vertices.insert(vertices.end(), {0.0f, 0.0f}); // UV
			vertices.insert(vertices.end(), {0.0f, 0.0f, 0.0f}); // Normal
		}
		// SumUp normals
		for (int i = 0; i < m_Indices.size()/3; ++i) {
			auto i1 = m_Indices[i * 3];
			auto i2 = m_Indices[i * 3 + 1];
			auto i3 = m_Indices[i * 3 + 2];
			vertices[i1 * 8 + 5] += normals[i].x;
			vertices[i1 * 8 + 6] += normals[i].y;
			vertices[i1 * 8 + 7] += normals[i].z;
			vertices[i2 * 8 + 5] += normals[i].x;
			vertices[i2 * 8 + 6] += normals[i].y;
			vertices[i2 * 8 + 7] += normals[i].z;
			vertices[i3 * 8 + 5] += normals[i].x;
			vertices[i3 * 8 + 6] += normals[i].y;
			vertices[i3 * 8 + 7] += normals[i].z;
		}
		// Get Average of each sum of normals
		for (int i = 0; i < vertices.size()/8; ++i) {
			auto vec = glm::vec3{vertices[i * 8 + 5], vertices[i * 8 + 6], vertices[i * 8 + 7]};
			vec = glm::normalize(vec);
			vertices[i * 8 + 5] = vec.x;
			vertices[i * 8 + 6] = vec.y;
			vertices[i * 8 + 7] = vec.z;
		}
		m_Vertices = vertices; // override original m_Vertices for later use at AABB
		auto ptr = std::make_shared<Lunar::Mesh>();
		ptr->CreateMesh(&(*vertices.begin()), &(*m_Indices.begin()), vertices.size(), m_Indices.size());
		m_MeshList.push_back(ptr);
	}

	~TestObject()
	{
		for (auto &itr : m_MeshList)
			itr->ClearMesh();
	}

	void Render(GLenum mode)
	{
		for (auto &itr : m_MeshList)
			itr->RenderMesh(mode);
	}

	glm::vec3 GetNormal(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
	{
		auto v1 = p2 - p1;
		auto v2 = p0 - p1;
		return glm::normalize(glm::cross(v1, v2));
	}
};

#endif//SCOOP_TESTOBJECT_H

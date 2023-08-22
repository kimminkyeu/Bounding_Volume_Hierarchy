//
// Created by Minkyeu Kim on 7/17/23.
//

#include "Mesh.h"

namespace AABB {

	Mesh::Mesh()
		: m_VAO(0), m_VBO(0), m_IBO(0), m_indexCount(0)
	{}

	Mesh::Mesh(GLfloat* verticies, unsigned int* indicies, unsigned int numOfVertices, unsigned int numOfIndicies)
			: m_VAO(0), m_VBO(0), m_IBO(0), m_indexCount(0)
	{
		this->CreateMesh(verticies, indicies, numOfVertices, numOfIndicies);
	}

	Mesh::~Mesh()
	{
		this->ClearMesh();
	}

	// https://docs.gl/
	void Mesh::CreateMesh(GLfloat* verticies, unsigned int* indicies, unsigned int numOfVertices, unsigned int numOfIndicies)
	{
		m_indexCount = numOfIndicies;
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
		// TexU TexV (stride = U ~ next U 까지의 거리)                            이건 xyz 지나고 첫 TexU 까지의 거리.
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(sizeof(GL_FLOAT) * 3));
//		glEnableVertexAttribArray(1);
//		// Normal xyz														    vertex x 부터 5칸 뒤가 첫 normal x
//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (void *)(sizeof(GL_FLOAT) * 5));
//		glEnableVertexAttribArray(2);

		// Unbind Buffer for later use
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unbind Vertex Array for later use
//		glBindVertexArray(0); // BUG: 이거 하면 안됨.... mac에서 오류 남...
	}

	void Mesh::RenderMesh(GLenum mode)
	{
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElements(mode, m_indexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Mesh::ClearMesh()
	{
		if (m_IBO != 0)
		{
			glDeleteBuffers(1, &m_IBO); // free-up GPU memory
			m_IBO = 0;
		}
		if (m_VBO != 0)
		{
			glDeleteBuffers(1, &m_VBO); // free-up GPU memory
			m_VBO = 0;
		}
		if (m_VAO != 0)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
		m_indexCount = 0;
	}
}

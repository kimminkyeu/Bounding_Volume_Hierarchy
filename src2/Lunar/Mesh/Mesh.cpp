//
// Created by Minkyeu Kim on 7/17/23.
//

#include "Mesh.h"

namespace Lunar {

	Mesh::Mesh()
		: m_VAO(0), m_VBO(0), m_IBO(0), m_indexCount(0)
	{}

	Mesh::Mesh(GLfloat* verticies, unsigned int* indicies, unsigned int numOfVertices, unsigned int numOfIndicies)
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

		// **************************************************
		// | Q1. 왜 bind 한 뒤 다시 unbind 하는지 도저히 이해가 안함. |
		// **************************************************
		// 음...
		// 여러 mesh 객체가 있다고 가정할 경우, (ex. Mesh A, Mesh B)
		// mesh A 고유의 VAO, VBO, IBO가 있을 것이고
		// mesh B 고유의 VAO, VBO, IBO가 있을 것이다.
		// 반면 그래픽스 파이프라인에서 사용하는 GL_***_BUFFER, VERTEX_ARRAY 는 GPU 상 단 한개이다.
		// 따라서 다른 객체의 계산을 위해서 항상 GL_****_BUFFER는 VERTEX_ARRAY는 unbind 주어야 한다...?

		// **************************************************************************************
		// (!) 추가 의문점... 그렇다면... 여기서 변수 m_VAO는 실제 데이터 저장 변수가 아닌, 그 버퍼의 ID, Name 이다.
		// 그렇다는 얘기는 우리가 malloc 하듯이, GPU의 메모리 영역에 해당 데이터를 할당한다는 이야기인가?
		// **************************************************************************************

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

		// Set vertex attribute : [ x y z float ]
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
		glEnableVertexAttribArray(0);

		// Unbind Buffer for later use
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unbind Vertex Array for later use
//		glBindVertexArray(0); // BUG: 이거 하면 안됨.... mac에서 오류 남...
	}

	void Mesh::RenderMesh()
	{
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
			glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
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

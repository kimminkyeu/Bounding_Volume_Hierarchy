//
// Created by Minkyeu Kim on 7/17/23.
//

#ifndef SCOOP_MESH_H
#define SCOOP_MESH_H

#include <GL/glew.h>

class Mesh
{
private:
	GLuint 	m_VAO;
	GLuint 	m_VBO;
	GLuint 	m_IBO;
	GLsizei	m_indexCount;
public:
	Mesh();

	~Mesh();

	void CreateMesh(GLfloat* verticies, unsigned int *indicies, unsigned int numOfVertices, unsigned int numOfIndicies);

	void RenderMesh();
	// delete data from GPU
	void ClearMesh();
};


#endif //SCOOP_MESH_H

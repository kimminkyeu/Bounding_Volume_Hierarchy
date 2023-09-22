//
// Created by Minkyeu Kim on 7/17/23.
//

#ifndef SCOOP_AABBMESH_H
#define SCOOP_AABBMESH_H

#include <GL/glew.h>

namespace AABB {

	class Mesh
	{
	protected:
		GLuint 	m_VAO;
		GLuint 	m_VBO;
		GLuint 	m_IBO;
		GLsizei	m_indexCount;
	public:
		Mesh();
		Mesh(GLfloat* verticies, unsigned int *indicies, unsigned int numOfVertices, unsigned int numOfIndicies);
		~Mesh();
		virtual void CreateMesh(GLfloat* verticies, unsigned int *indicies, unsigned int numOfVertices, unsigned int numOfIndicies);
		void RenderMesh(GLenum mode = GL_TRIANGLES);
		void ClearMesh(); // delete data from GPU
	};
}

#endif //SCOOP_AABBMESH_H

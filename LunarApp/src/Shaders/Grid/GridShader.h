//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_GRIDSHADER_H
#define SCOOP_GRIDSHADER_H

#include "Lunar/Shader/Shader.h"

class GridShader : public Lunar::Shader
{
public:
	glm::vec4 m_GridColor = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	float m_FarClip = 1000.0f;
	float m_NearClip = 0.01f;

private:
	GLint m_GridColorLocation = 0;
	GLint m_FarLocation = 0;
	GLint m_NearLocation = 0;
	GLuint m_DummyVAO = 0; // OpenGL Core는 VAO 0을 오브젝트 없음으로 해석함.

public:
	GridShader()
		: Lunar::Shader("Grid",
						"LunarApp/src/shaders/Grid/vertex_shader.glsl",
						"LunarApp/src/shaders/Grid/fragment_shader.glsl"
		  )
				  {
					  m_GridColorLocation = this->_GetUniformLocation("GridColor");
					  m_FarLocation = this->_GetUniformLocation("FarClip"); // for gid fade-out
					  m_NearLocation = this->_GetUniformLocation("NearClip"); // for gid fade-out
					  glGenVertexArrays(1, &m_DummyVAO);
				  };


private:
	 void OnBind() override
	 {
		 glEnable(GL_PROGRAM_POINT_SIZE);
		 glUniform4fv(m_GridColorLocation, 1, glm::value_ptr(m_GridColor));
		 glUniform1f(m_FarLocation, m_FarClip);
		 glUniform1f(m_NearLocation, m_NearClip);
	 };
	 void OnUnbind() override
	 {
		 glDisable(GL_PROGRAM_POINT_SIZE);
	 };

public:
	 void DrawDummyVAO()
	 {
		 // NOTE: bind dummy vao just for grid.
		 glBindVertexArray(m_DummyVAO);
//		 glDrawArrays(GL_TRIANGLES, 0, 6);
		 glDrawArrays(GL_TRIANGLES, 0, 6);
	 }
};

#endif//SCOOP_GRIDSHADER_H

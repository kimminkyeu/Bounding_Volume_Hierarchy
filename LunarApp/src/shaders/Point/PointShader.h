//
// Created by USER on 2023-08-10.
//

#ifndef SCOOP_POINTSHADER_H
#define SCOOP_POINTSHADER_H

#include "Lunar/Shader/Shader.h"

class PointShader : public Lunar::Shader
{
public:
	glm::vec4 m_PointColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
private:
	GLint m_PointColorLocation = 0;
public:
	PointShader()
			: Lunar::Shader("Point",
							"LunarApp/src/shaders/Point/vertex_shader.glsl",
							"LunarApp/src/shaders/Point/fragment_shader.glsl"
	)
	{
		m_PointColorLocation = this->_GetUniformLocation("PointColor");
	};

	~PointShader() {};

private:
	void OnBind() override
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glUniform4fv(m_PointColorLocation, 1, glm::value_ptr(m_PointColor));
	};

	void OnUnbind() override
	{
		glDisable(GL_PROGRAM_POINT_SIZE);
	};
};

#endif//SCOOP_POINTSHADER_H

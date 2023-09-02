//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_AABBSHADER_H
#define SCOOP_AABBSHADER_H

#include "Lunar/Shader/Shader.h"

class AABBShader : public Lunar::Shader
{
public:
	glm::vec4 m_AABBColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
private:
	GLint m_AABBColorLocation = 0;
public:
	AABBShader()
		: Lunar::Shader("AABB",
						"LunarApp/src/shaders/AABB/vertex_shader.glsl",
						"LunarApp/src/shaders/AABB/fragment_shader.glsl",
						"LunarApp/src/shaders/AABB/geometry_shader.glsl"
		  )
				  {
					  m_AABBColorLocation = this->_GetUniformLocation("AABBColor");
				  };


private:
	 void OnBind() override
	 {
		 glUniform4fv(m_AABBColorLocation, 1, glm::value_ptr(m_AABBColor));
	 };
	 void OnUnbind() override
	 {};
};

#endif//SCOOP_AABBSHADER_H

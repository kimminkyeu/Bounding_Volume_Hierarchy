//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_WIREFRAMESHADER_H
#define SCOOP_WIREFRAMESHADER_H

#include "Lunar/Shader/Shader.h"

class WireframeShader : public Lunar::Shader
{
public:
	glm::vec4 m_WireframeColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
private:
	GLint m_WireframeColorLocation = 0;
public:
	WireframeShader()
		: Lunar::Shader("Wireframe",
						"LunarApp/src/shaders/Wireframe/vertex_shader.glsl",
						"LunarApp/src/shaders/Wireframe/fragment_shader.glsl",
						"LunarApp/src/shaders/Wireframe/geometry_shader.glsl"
		  )
				  {
					  m_WireframeColorLocation = this->_GetUniformLocation("WireframeColor");
				  };


private:
	 void OnBind() override
	 {
		 glUniform4fv(m_WireframeColorLocation, 1, glm::value_ptr(m_WireframeColor));
	 };
	 void OnUnbind() override
	 {};
};

#endif//SCOOP_WIREFRAMESHADER_H

//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_NORMALSHADER_H
#define SCOOP_NORMALSHADER_H

#include "Lunar/Shader/Shader.h"

class NormalShader : public Lunar::Shader
{
public:
	NormalShader()
		: Lunar::Shader("Normal",
						"LunarApp/src/Shaders/Normal/vertex_shader.glsl",
						"LunarApp/src/Shaders/Normal/fragment_shader.glsl",
						"LunarApp/src/Shaders/Normal/geometry_shader.glsl"
		  ) {};

private:
	void OnBind() override {};
	void OnUnbind() override {};
};



#endif//SCOOP_NORMALSHADER_H

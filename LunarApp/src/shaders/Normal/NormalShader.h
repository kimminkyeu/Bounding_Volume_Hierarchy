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
						"LunarApp/src/shaders/Normal/vertex_shader.glsl",
						"LunarApp/src/shaders/Normal/fragment_shader.glsl",
						"LunarApp/src/shaders/Normal/geometry_shader.glsl"
		  ) {};

	void BindDataToGPU() override {};
};



#endif//SCOOP_NORMALSHADER_H

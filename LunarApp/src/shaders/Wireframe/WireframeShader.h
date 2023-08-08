//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_WIREFRAMESHADER_H
#define SCOOP_WIREFRAMESHADER_H

#include "Lunar/Shader/Shader.h"

class WireframeShader : public Lunar::Shader
{
public:
	WireframeShader()
		: Lunar::Shader("Wireframe",
						"LunarApp/src/shaders/Wireframe/vertex_shader.glsl",
						"LunarApp/src/shaders/Wireframe/fragment_shader.glsl",
						"LunarApp/src/shaders/Wireframe/geometry_shader.glsl"
		  )
				  {};


	 void BindDataToGPU() override {};
};

#endif//SCOOP_WIREFRAMESHADER_H

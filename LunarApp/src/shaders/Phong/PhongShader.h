//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_PHONGSHADER_H
#define SCOOP_PHONGSHADER_H

// 해당 쉐이더에 관련된 변수는 여기에서 저장.
#include "Lunar/Shader/Shader.h"

class PhongShader : public Lunar::Shader
{
public:
	PhongShader()
			: Lunar::Shader("Phong",
							"LunarApp/src/shaders/Phong/vertex_shader.glsl",
							"LunarApp/src/shaders/Phong/fragment_shader.glsl"
	)
	{

	};

	void OnBind() override {};
	void OnUnbind() override {};
};


#endif//SCOOP_PHONGSHADER_H

//
// Created by USER on 2023-08-10.
//

#ifndef SCOOP_CARTOONSHADER_H
#define SCOOP_CARTOONSHADER_H

// 해당 쉐이더에 관련된 변수는 여기에서 저장.
#include "Lunar/Shader/Shader.h"

class CartoonShader : public Lunar::Shader
{
public:
	CartoonShader()
			: Lunar::Shader("Cartoon",
							"LunarApp/src/shaders/Cartoon/vertex_shader.glsl",
							"LunarApp/src/shaders/Cartoon/fragment_shader.glsl"
	)
	{

	};

	void OnBind() override {};
	void OnUnbind() override {};
};

#endif//SCOOP_CARTOONSHADER_H

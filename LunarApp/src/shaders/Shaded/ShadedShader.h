//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_SHADEDSHADER_H
#define SCOOP_SHADEDSHADER_H

// 해당 쉐이더에 관련된 변수는 여기에서 저장.
#include "Lunar/Shader/Shader.h"

class ShadedShader : public Lunar::Shader
{
public:
	ShadedShader()
		: Lunar::Shader("Shaded",
									"LunarApp/src/shaders/Shaded/vertex_shader.glsl",
									"LunarApp/src/shaders/Shaded/fragment_shader.glsl"
						)
	{
	}

	void BindDataToGPU() override {}
};

#endif

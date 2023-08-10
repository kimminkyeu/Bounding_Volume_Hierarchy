//
// Created by USER on 2023-08-10.
//

#ifndef SCOOP_POINTSHADER_H
#define SCOOP_POINTSHADER_H

#include "Lunar/Shader/Shader.h"

class PointShader : public Lunar::Shader
{
public:
	PointShader()
			: Lunar::Shader("Point",
							"LunarApp/src/shaders/Point/vertex_shader.glsl",
							"LunarApp/src/shaders/Point/fragment_shader.glsl"
	)
	{};

	~PointShader() {};

private:
	void OnBind() override
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
	};

	void OnUnbind() override
	{
		glDisable(GL_PROGRAM_POINT_SIZE);
	};
};

#endif//SCOOP_POINTSHADER_H

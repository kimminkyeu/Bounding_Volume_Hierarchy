//
// Created by Minkyeu Kim on 8/8/23.
//

#ifndef SCOOP_PHONGSHADER_H
#define SCOOP_PHONGSHADER_H

// 해당 쉐이더에 관련된 변수는 여기에서 저장.
#include "Lunar/Shader/Shader.h"

class PhongShader : public Lunar::Shader
{
private:
	int m_PickMode = 0; // NOTE: 0 = off
	glm::vec3 m_PickedMeshV0; // V0
	glm::vec3 m_PickedMeshV1; // V1
	glm::vec3 m_PickedMeshV2; // V2

	GLint m_PickModeLocation; // shader location of picked mesh info
	GLint m_PickedMeshV0Location; // shader location of picked mesh info
	GLint m_PickedMeshV1Location; // shader location of picked mesh info
	GLint m_PickedMeshV2Location; // shader location of picked mesh info

public:
	PhongShader()
			: Lunar::Shader("Phong",
							"LunarApp/src/shaders/Phong/vertex_shader.glsl",
							"LunarApp/src/shaders/Phong/fragment_shader.glsl"
	)
	{
		m_PickModeLocation = this->_GetUniformLocation("PickMode");
		m_PickedMeshV0Location = this->_GetUniformLocation("PickedMeshV0");
		m_PickedMeshV1Location = this->_GetUniformLocation("PickedMeshV1");
		m_PickedMeshV2Location = this->_GetUniformLocation("PickedMeshV2");
	};

	// set picked mesh data to
	void SetPickedMeshData(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
	{
		this->m_PickedMeshV0 = v0;
		this->m_PickedMeshV1 = v1;
		this->m_PickedMeshV2 = v2;
	}

	void SetPickMode(int mode)
	{ this->m_PickMode = mode; }

	void OnBind() override
	{
		glUniform1i(m_PickModeLocation, m_PickMode);
		glUniform3fv(m_PickedMeshV0Location, 1, glm::value_ptr(m_PickedMeshV0));
		glUniform3fv(m_PickedMeshV1Location, 1, glm::value_ptr(m_PickedMeshV1));
		glUniform3fv(m_PickedMeshV2Location, 1, glm::value_ptr(m_PickedMeshV2));
	};

	void OnUnbind() override
	{};

};


#endif//SCOOP_PHONGSHADER_H

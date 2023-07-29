//
// Created by USER on 2023-07-29.
//

#include <GL/glew.h>

#ifndef SCOOP_MATERIAL_H
#define SCOOP_MATERIAL_H

#include <glm/glm.hpp>
#include "Lunar/Shader/ShaderProgram.h"

namespace Lunar {
	class Material
	{
	public:
		Material() = default;
		~Material() = default;

	public:
		// set each member data to uniform shader variable
		void UseMaterial(const Lunar::ShaderProgram& shader) const;

	private:
		glm::vec3	m_SpecularColor 	= glm::vec3(1.0f);	// Ks
		glm::vec3	m_AmbientColor 		= glm::vec3(1.0f);	// Ka
		glm::vec3	m_DiffuseColor 		= glm::vec3(1.0f);	// Kd
		float		m_SpecularExponent 	= 90.0f; 	// Ns
		float		m_IndexOfRefraction = 1.0f;		// Ni
		float		m_Dissolve 			= 1.0f;		// d
		float		m_IlluminationModel = 2;		// illum

	public:
		inline void SetSpecularColor(const glm::vec3& rgb) { m_SpecularColor = rgb; }
		inline void SetAmbientColor(const glm::vec3& rgb) { m_AmbientColor = rgb; }
		inline void SetDiffuseColor(const glm::vec3& rgb) { m_DiffuseColor = rgb; }
		inline void SetSpecularExponent(float ns) { m_SpecularExponent = ns; }
		inline void SetIndexOfRefraction(float ni) { m_IndexOfRefraction = ni; }
		inline void SetDissolve(float d) { m_Dissolve = d; }
		inline void SetIlluminationModel(float illum) { m_IlluminationModel = illum; }
	};
}



#endif //SCOOP_MATERIAL_H

//
// Created by USER on 2023-07-29.
//

#include <GL/glew.h>

#ifndef SCOOP_MATERIAL_H
#define SCOOP_MATERIAL_H

#include "Lunar/Shader/Shader.h"
#include <glm/glm.hpp>

namespace Lunar {
	class Material
	{
	public:
		explicit Material(const glm::vec3& ambient = glm::vec3(1.0f), 	// Ks
				 const glm::vec3& diffuse = glm::vec3(1.0f), 	// Ka
				 const glm::vec3& specular = glm::vec3(1.0f),	// Kd
				 float specularExponent = 90.0f, 						// Ns
				 float IOR = 1.0f, 										// Ni
				 float dissolve = 1.0f, 									// d
				 float illuminationModel = 2); 							// illum
		~Material() = default;

	public:
		// set each member data to uniform shader variable
		void UseMaterial(const Lunar::Shader& shader) const;

	public:
		glm::vec3	m_SpecularColor;
		glm::vec3	m_AmbientColor;
		glm::vec3	m_DiffuseColor;
		float		m_SpecularExponent;
		float		m_IndexOfRefraction;
		float		m_Dissolve;
		float		m_IlluminationModel;

//	public:
//		inline void SetSpecularColor(const glm::vec3& rgb) { m_SpecularColor = rgb; }
//		inline void SetAmbientColor(const glm::vec3& rgb) { m_AmbientColor = rgb; }
//		inline void SetDiffuseColor(const glm::vec3& rgb) { m_DiffuseColor = rgb; }
//		inline void SetSpecularExponent(float ns) { m_SpecularExponent = ns; }
//		inline void SetIndexOfRefraction(float ni) { m_IndexOfRefraction = ni; }
//		inline void SetDissolve(float d) { m_Dissolve = d; }
//		inline void SetIlluminationModel(float illum) { m_IlluminationModel = illum; }
	};
}



#endif //SCOOP_MATERIAL_H

//
// Created by USER on 2023-07-29.
//

#include "Material.h"

namespace Lunar {

	Material::Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float specularExponent, float IOR, float dissolve, float illuminationModel)
		:  m_AmbientColor(ambient), m_DiffuseColor(diffuse), m_SpecularColor(specular), m_SpecularExponent(specularExponent),
		  m_IndexOfRefraction(IOR), m_Dissolve(dissolve), m_IlluminationModel(illuminationModel)
	{}

	void Material::Use(const Lunar::Shader *shaderPtr) const
	{
        // shader의 unform 변수들 위치 얻어내기
		const auto locs = shaderPtr->GetUniformMaterialLocations();
		glUniform3f(locs.SpecularColorLocation, m_SpecularColor.r, m_SpecularColor.g, m_SpecularColor.b);
		glUniform3f(locs.AmbientColorLocation, m_AmbientColor.r, m_AmbientColor.g, m_AmbientColor.b);
		glUniform3f(locs.DiffuseColorLocation, m_DiffuseColor.r, m_DiffuseColor.g, m_DiffuseColor.b);
		glUniform1f(locs.SpecularExponentLocation, m_SpecularExponent);
		glUniform1f(locs.IndexOfRefractionLocation, m_IndexOfRefraction);
		glUniform1f(locs.DissolveLocation, m_Dissolve);
		glUniform1f(locs.IlluminationModelLocation, m_IlluminationModel);
	}
}


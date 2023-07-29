//
// Created by USER on 2023-07-29.
//

#include "Material.h"

namespace Lunar {

	void Material::UseMaterial(const ShaderProgram& shader) const
	{
		const auto locs = shader.GetUniformMaterial();
		glUniform3f(locs.SpecularColorLocation, m_SpecularColor.r, m_SpecularColor.g, m_SpecularColor.b);
		glUniform3f(locs.AmbientColorLocation, m_AmbientColor.r, m_AmbientColor.g, m_AmbientColor.b);
		glUniform3f(locs.DiffuseColorLocation, m_DiffuseColor.r, m_DiffuseColor.g, m_DiffuseColor.b);
		glUniform1f(locs.SpecularExponentLocation, m_SpecularExponent);
		glUniform1f(locs.IndexOfRefractionLocation, m_IndexOfRefraction);
		glUniform1f(locs.DissolveLocation, m_Dissolve);
		glUniform1f(locs.IlluminationModelLocation, m_IlluminationModel);
	}
}


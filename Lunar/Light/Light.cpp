//
// Created by Minkyeu Kim on 7/25/23.
//

#include "Light.h"

namespace Lunar {

	Light::Light(const glm::vec3& direction, float ambientIntensity, float diffuseIntensity, float specularIntensity)
		: m_Direction(direction), m_AmbientIntensity(ambientIntensity), m_DiffuseIntensity(diffuseIntensity), m_SpecularIntensity(specularIntensity)
	{}

	void Light::UseLight(const Lunar::Shader& shader)
	{
		// Set value of uniform variable inside GPU
		auto locs = shader.GetUniformDirectionLight();
		glUniform1f(locs.AmbientIntensityLocation, m_AmbientIntensity);
		glUniform3f(locs.DirectionLocation,m_Direction.x, m_Direction.y, m_Direction.z);
		glUniform1f(locs.DiffuseIntensityLocation, m_DiffuseIntensity);
		glUniform1f(locs.SpecularIntensityLocation, m_SpecularIntensity);
	}

	Light::~Light()
	{}
}

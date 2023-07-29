//
// Created by Minkyeu Kim on 7/25/23.
//

#include "Light.h"

namespace Lunar {

	Light::Light()
		: m_AmbientColor(glm::vec3(1.0f, 1.0f, 1.0f)), m_AmbientIntensity(1.0f), m_Direction(glm::vec3(0.0f, -1.0f, 0.0f)), m_DiffuseIntensity(0.0f),
	m_SpecularIntensity(0.0f)
	{}

	Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity, GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity, GLfloat specularIntensity)
		: m_AmbientColor(glm::vec3(red, green, blue)), m_AmbientIntensity(ambientIntensity), m_Direction(glm::vec3(xDir, yDir, zDir)), m_DiffuseIntensity(diffuseIntensity),
	m_SpecularIntensity(specularIntensity)
	{}

	void Light::UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation, GLint diffuseIntensityLocation, GLint directionLocation, GLint specularIntensityLocation)
	{
		// set value of uniform variable inside GPU
		glUniform3f(ambientColorLocation,m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z);
		glUniform1f(ambientIntensityLocation, m_AmbientIntensity);

		glUniform3f(directionLocation,m_Direction.x, m_Direction.y, m_Direction.z);
		glUniform1f(diffuseIntensityLocation, m_DiffuseIntensity);

		glUniform1f(specularIntensityLocation, m_SpecularIntensity);
	}

	Light::~Light()
	{

	}
}

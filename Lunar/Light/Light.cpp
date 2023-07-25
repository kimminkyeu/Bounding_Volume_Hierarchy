//
// Created by Minkyeu Kim on 7/25/23.
//

#include "Light.h"

namespace Lunar {

	Light::Light()
		: m_AmbientColor(glm::vec3(1.0f, 1.0f, 1.0f)), m_AmbientIntensity(1.0f)
	{}

	Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity)
		: m_AmbientColor(glm::vec3(red, green, blue)), m_AmbientIntensity(ambientIntensity)
	{}

	void Light::UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation)
	{
		// set value of uniform variable inside GPU
		glUniform3f(ambientColorLocation,m_AmbientColor.x, m_AmbientColor.y, m_AmbientColor.z);
		glUniform1f(ambientIntensityLocation, m_AmbientIntensity);
	}

	Light::~Light()
	{

	}

}

//
// Created by USER on 2023-07-26.
//

#ifndef SCOOP_LIGHT_H
#define SCOOP_LIGHT_H

#include <GL/glew.h>
#include <glm.hpp>

namespace Lunar {
	class Light
	{
	private:
		glm::vec3 m_Direction; // light direction
		glm::vec3 m_AmbientColor;
		float m_AmbientIntensity;
		float m_DiffuseIntensity;
		float m_SpecularIntensity;

	public:
		Light();
		Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity,
			  GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat diffuseIntensity, GLfloat specularIntensity);
		~Light();
		void UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation,
					  GLint diffuseIntensityLocation, GLint directionLocation, GLint specularIntensityLocation);
	};
}


#endif //SCOOP_LIGHT_H

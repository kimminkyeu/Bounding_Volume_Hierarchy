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
		glm::vec3 m_AmbientColor;
		float m_AmbientIntensity;


	public:
		Light();
		Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat ambientIntensity);
		~Light();
		void UseLight(GLint ambientIntensityLocation, GLint ambientColorLocation);
	};
}


#endif //SCOOP_LIGHT_H

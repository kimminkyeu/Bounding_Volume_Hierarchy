//
// Created by USER on 2023-07-26.
//

#ifndef SCOOP_LIGHT_H
#define SCOOP_LIGHT_H

#include "Lunar/Shader/Shader.h"
#include <GL/glew.h>
#include <glm.hpp>

namespace Lunar {
	class Light
	{
	private:
		glm::vec3 m_Direction = glm::vec3(0.0, 0.0f, -1.0f);
		float m_AmbientIntensity = 1.0f;
		float m_DiffuseIntensity = 1.0f;
		float m_SpecularIntensity = 1.0f;

	public:
		Light() = default;
		Light(const glm::vec3& direction, float ambientIntensity, float diffuseIntensity, float specularIntensity);
		~Light();
		void Use(const Lunar::Shader* shaderPtr);

		inline void SetDirection(const glm::vec3& dir) { m_Direction = dir; }
		inline void SetAmbientIntensity(float intensity) { m_AmbientIntensity = intensity; }
		inline void SetDiffuseIntensity(float intensity) { m_DiffuseIntensity = intensity; }
		inline void SetSpecularIntensity(float intensity) { m_SpecularIntensity = intensity; }

		inline float GetAmbientIntensity() const { return m_AmbientIntensity; }
		inline float GetDiffuseIntensity() const { return m_DiffuseIntensity; }
		inline float GetSpecularIntensity() const { return m_SpecularIntensity; }
		inline glm::vec3 GetDirection() const { return m_Direction; }
	};
}


#endif //SCOOP_LIGHT_H

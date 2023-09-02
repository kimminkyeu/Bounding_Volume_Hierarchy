//
// Created by USER on 2023-07-24.
//

#ifndef SCOOP_CAMERA_H
#define SCOOP_CAMERA_H

#include <glm/glm.hpp>

namespace Lunar {
	class CameraBase
	{
	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);

	public:
		CameraBase() = default;
		CameraBase(const glm::mat4& projection)
			: m_Projection(projection) {}
		virtual ~CameraBase() = default;
		const glm::mat4& GetProjection() const { return m_Projection; };
	};
}

#endif //SCOOP_CAMERA_H

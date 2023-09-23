//
// Created by Minkyeu Kim on 9/23/23.
//

#ifndef SCOOP_UTILS_H
#define SCOOP_UTILS_H

#include "glm/glm.hpp"

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
		uint8_t g = (uint8_t)(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
		uint8_t b = (uint8_t)(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
		uint8_t a = (uint8_t)(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

#endif//SCOOP_UTILS_H

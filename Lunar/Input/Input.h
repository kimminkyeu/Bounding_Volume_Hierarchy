//
// Created by Minkyeu Kim on 7/22/23.
//

#ifndef SCOOP_INPUT_H
#define SCOOP_INPUT_H

#include "KeyCodes.h"
#include "MouseCodes.h"

#include <glm/glm.hpp>

namespace Lunar {

    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode keycode);
        static bool IsMouseButtonPressed(MouseCode button);
		static bool IsMouseButtonReleased(MouseCode code);
		static bool IsMouseButtonPressedOnce(MouseCode code);
        static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}

#endif //SCOOP_INPUT_H

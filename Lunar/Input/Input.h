//
// Created by Minkyeu Kim on 7/22/23.
//

#ifndef SCOOP_INPUT_H
#define SCOOP_INPUT_H

#include "KeyCodes.h"

#include <glm/glm.hpp>

namespace Lunar {

    class Input
    {
    public:
        static bool IsKeyDown(KeyCode keycode);
        static bool IsMouseButtonDown(MouseButton button);

        static glm::vec2 GetMousePosition();

        static void SetCursorMode(CursorMode mode);
    };

}

#endif //SCOOP_INPUT_H

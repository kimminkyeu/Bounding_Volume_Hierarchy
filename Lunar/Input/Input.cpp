//
// Created by Minkyeu Kim on 7/22/23.
//
#include "Input.h"
#include "Lunar/Core/Application.h"
#include <GLFW/glfw3.h>

// https://github.com/StudioCherno/Walnut/blob/master/Walnut/src/Walnut/Input/Input.cpp
namespace Lunar {

    bool Input::IsKeyPressed(Lunar::KeyCode keycode)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetKey(windowHandle, (int)keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(MouseCode code)
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
        int state = glfwGetMouseButton(windowHandle, (int)code);
        return state == GLFW_PRESS;
    }

	bool Input::IsMouseButtonPressedOnce(MouseCode code)
	{
		static bool down = false;
		GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
		int state = glfwGetMouseButton(windowHandle, (int)code);
		if (!down && state == GLFW_PRESS) { // if only button up
			down = true;
			return true;
		}
		if (down && state == GLFW_RELEASE) down = false;
		return false;
	}

	bool Input::IsMouseButtonReleased(MouseCode code)
	{
		GLFWwindow* windowHandle = Application::Get().GetWindowHandle();
		int state = glfwGetMouseButton(windowHandle, (int)code);
		return state == GLFW_RELEASE;
	}

    glm::vec2 Input::GetMousePosition()
    {
        GLFWwindow* windowHandle = Application::Get().GetWindowHandle();

        double x, y;
        glfwGetCursorPos(windowHandle, &x, &y);
        return { (float)x, (float)y };
    }

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}
}
//
// Created by Minkyeu Kim on 7/21/23.
//

#include "Application.h"

// Reference
// https://github.com/StudioCherno/Walnut/blob/3b8e414fdecfc6c8b58816106fe8d912bd172e31/Walnut/src/Walnut/Application.cpp
// 위 코드를 OpenGL 버전으로 수정하였습니다.
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
//#include <utility>
#include <stdio.h> // fprintf

extern bool g_ApplicationRunning;
static Lunar::Application* s_Instance = nullptr; // Single instance

static void glfw_error_callback(int error_code, const char* description) noexcept
{
	fprintf(stderr, "GLFW Error %d: %s\n", error_code, description);
}

namespace Lunar {
	// https://stackoverflow.com/questions/51705967/advantages-of-pass-by-value-and-stdmove-over-pass-by-reference
	// r-value 와 l-value 모두를 Cover 하는 생성자 코드.
	Application::Application(Lunar::ApplicationSpecification  appSpec) noexcept
		: m_Specification(std::move(appSpec))
	{
		s_Instance = this;
		Init();
	}

	/* static */ Application& Application::Get() noexcept
	{
		return *s_Instance;
	}

	void Application::Init() noexcept
	{
		// Setup GLFW window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) // Init GLFW
		{
			std::cerr << "GLFW initialization failed...\n";
			return;
		}

	}

	Application::~Application() noexcept
	{

	}
}
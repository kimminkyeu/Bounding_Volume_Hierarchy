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
#include "Log.h"

extern bool g_ApplicationRunning;
static Lunar::Application* s_Instance = nullptr; // Single instance

static void glfw_error_callback(int error_code, const char* description) noexcept
{
	fprintf(stderr, "GLFW Error %d: %s\n", error_code, description);
}

namespace Lunar {

	Application::Application(Lunar::ApplicationSpecification  appSpec) noexcept
		: m_Specification(std::move(appSpec))
	{   // https://stackoverflow.com/questions/51705967/advantages-of-pass-by-value-and-stdmove-over-pass-by-reference
        if (s_Instance == nullptr) // 이미 app이 하나 켜져있으면 더이상 키지 말 것!
        {
            s_Instance = this;
            Init();
        }
        else
        {
            LOG_ERROR("Duplicate Application instance");
            assert(false);
        }
	}

    Application::~Application() noexcept
    {
        this->Shutdown();
        s_Instance = nullptr;
    }

	//	https://github.com/TheCherno/OpenGL/blob/master/OpenGL-Core/src/Platform/Windows/WindowsWindow.cpp#L20
	void Application::Init() noexcept
	{
		// Setup GLFW window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) // Init GLFW
		{
			std::cerr << "GLFW initialization failed...\n";
			return;
		}
		// Setup GLFW window properties
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL major version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL minor version
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile = No Backwards Compatibility
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Allow forward compatibility

		m_Window.Handle = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Specification.Name, nullptr, nullptr);
		if (!m_Window.Handle)
		{
			glfwTerminate();
			assert(false && "GLFW window creation failed");
		}
		// set the current context
		glfwMakeContextCurrent(m_Window.Handle); // Set context for GLEW to use (그럼 multiple context 존재? 여러 화면?)

		LOG_INFO("OpenGL Info:");
		LOG_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		LOG_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		LOG_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		// 윈도우에 따라 getWindowUserPointer가 반환해줄 값을 설정(나를 위함) --> 이후 callback에서 사용.
		glfwSetWindowUserPointer(m_Window.Handle, &m_Window);
		// [SWAP BUFFER SYNC] : https://www.glfw.org/docs/3.3/quick.html#quick_swap_buffers
		glfwSwapInterval(1);
		// Get Buffer size information (data for window rendering) : https://www.glfw.org/docs/3.3/window_guide.html#window_fbsize
		glfwGetFramebufferSize(m_Window.Handle, &m_Window.BufferWidth, &m_Window.BufferHeight);

		// set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window.Handle,[](GLFWwindow* currentWindow, int width, int height) -> void
		{
			auto currentWindowDataPtr = (Lunar::WindowData *)glfwGetWindowUserPointer(currentWindow);
			currentWindowDataPtr->BufferWidth = width;
			currentWindowDataPtr->BufferHeight = height;
			LOG_TRACE("Window Resize: W={0} H={0}", width, height);
			// TODO: Add appropriate event callback. Ex) window 100, height 200일 때 특정 event 호출!
		});

        // set GLFW callbacks
		glfwSetWindowCloseCallback(m_Window.Handle,[](GLFWwindow* currentWindow) -> void
		{
            g_ApplicationRunning = false;
			LOG_WARN("Closing Window...");
		});

        // set GLFW callbacks
        glfwSetMouseButtonCallback(m_Window.Handle, [](GLFWwindow* window, int button, int action, int mode) -> void
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    LOG_TRACE("Pressed {0}", button);
                    break;
                }
                case GLFW_RELEASE:
                {
                    LOG_TRACE("Released {0}", button);
                    break;
                }
                case GLFW_REPEAT:
                {
                    LOG_TRACE("Repeated {0}", button);
                    break;
                }
                default:
                {
                    // ...
                }
            }
        });

        // set GLFW callbacks
        glfwSetCursorPosCallback(m_Window.Handle, [](GLFWwindow* currentWindow, double xPos, double yPos) -> void
        {
            LOG_TRACE("Mouse move X={0} Y={0}", xPos, yPos);
        });

        // set GLFW callbacks
        glfwSetKeyCallback(m_Window.Handle,[](GLFWwindow* currentWindow, int key, int code, int action, int mode) -> void
        {
            switch (action)
            {
                case GLFW_PRESS:
                {
                    LOG_TRACE("Pressed {0}", key);
                    break;
                }
                case GLFW_RELEASE:
                {
                    LOG_TRACE("Released {0}", key);
                    break;
                }
                case GLFW_REPEAT:
                {
                    LOG_TRACE("Repeated {0}", key);
                    break;
                }
                default:
                {
                    // ...
                }
            }
        });


        // TODO: remove later!
        glewExperimental = GL_TRUE; // Allow modern extension features
        if (glewInit() != GLEW_OK) // Check init state
        {
            glfwDestroyWindow(m_Window.Handle);
            glfwTerminate();
            assert(false && "glew initialization failed");
        }
        // TODO: remove later!
        glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
        glViewport(0, 0, m_Window.BufferWidth, m_Window.BufferHeight); // Setup Viewport size (OpenGL functionality)
    }

    GLFWwindow* Application::GetWindowHandle() const noexcept
    {
        return m_Window.Handle;
    }

    // https://github.com/StudioCherno/Walnut/blob/20f940b9d23946d4836b8549ff3e2c0750c5d985/Walnut/src/Walnut/Application.cpp#L554
    void Application::Run() noexcept
    {
        m_Running = true;
        while (!glfwWindowShouldClose(m_Window.Handle) && m_Running)
        {
            // Poll and handle events (inputs, window resize, etc.)
            glfwPollEvents();
            for (auto& layer : m_LayerStack) {
                layer->OnUpdate(m_TimeStep);
            }
            // ...

            // update time past (for animation)
            float time = this->GetTime();
            m_FrameTime = time - m_LastFrameTime;
            m_TimeStep = glm::min<float>(m_FrameTime, 0.0333f);
            m_LastFrameTime = time;
        }
    }

    void Application::Close() noexcept
    {
        m_Running = false;
    }

    void Application::Shutdown() noexcept {
        // clear stack
        for (auto& layer : m_LayerStack) {
            layer->OnDetach();
        }
        m_LayerStack.clear();
        glfwDestroyWindow(m_Window.Handle);
        glfwTerminate();
        g_ApplicationRunning = false;
    }

    /* static function */
    float Application::GetTime() noexcept
    {
        return (float)glfwGetTime();
    }

    /* static function */
    Application& Application::Get() noexcept
    {
        return *s_Instance;
    }
}
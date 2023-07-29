//
// Created by Minkyeu Kim on 7/21/23.
//

// https://github.com/StudioCherno/Walnut/blob/3b8e414fdecfc6c8b58816106fe8d912bd172e31/Walnut/src/Walnut/Application.cpp
#include "Application.h"
#include "Log.h" // 이건 cpp에만 설정.

extern bool g_ApplicationRunning;
static Lunar::Application* s_Instance = nullptr; // Single instance

static void glfw_error_callback(int error_code, const char* description) noexcept
{
	fprintf(stderr, "GLFW Error %d: %s\n", error_code, description);
}

namespace Lunar {

	Application::Application(const Lunar::ApplicationSpecification& appSpec) noexcept
		: m_Specification(appSpec)
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

	// Initialize GLFW window
	// https://github.com/TheCherno/OpenGL/blob/master/OpenGL-Core/src/Platform/Windows/WindowsWindow.cpp#L20
	void Application::Init() noexcept
	{
		// Setup GLFW window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) // Init GLFW
		{
			LOG_ERROR("GLFW initialization failed...");
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
		glfwSetWindowUserPointer(m_Window.Handle, this);
		// [SWAP BUFFER SYNC] : https://www.glfw.org/docs/3.3/quick.html#quick_swap_buffers
		glfwSwapInterval(1);
		// Get Buffer size information (data for window rendering) : https://www.glfw.org/docs/3.3/window_guide.html#window_fbsize
		glfwGetFramebufferSize(m_Window.Handle, &m_Window.BufferWidth, &m_Window.BufferHeight);

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

		// set GLFW callbacks
		// 아놔 복잡하네... 이거 어떻게 처리하니...
		glfwSetWindowSizeCallback(m_Window.Handle,[](GLFWwindow* currentWindow, int width, int height) -> void
		{
			auto app = (Lunar::Application *)glfwGetWindowUserPointer(currentWindow);
			app->m_Window.BufferWidth = width;
			app->m_Window.BufferHeight = height;
			LOG_TRACE("Window Resize: W={0} H={0}", width, height);
			glViewport(0, 0, width, height);
			for (auto &layer : app->m_LayerStack)
			{
				layer->OnWindowResize(width, height);
			}
		});

        // set GLFW callbacks
		glfwSetWindowCloseCallback(m_Window.Handle,[](GLFWwindow* window) -> void
		{
			auto app = (Lunar::Application *)glfwGetWindowUserPointer(window);
			app->Shutdown();
		});

		glfwSetScrollCallback(m_Window.Handle, [](GLFWwindow* window, double xOffset, double yOffset) -> void
		{
			LOG_TRACE("Mouse Scroll: xOffset={0} yOffset={0}", xOffset, yOffset);
		});

        // set GLFW callbacks
        glfwSetMouseButtonCallback(m_Window.Handle, [](GLFWwindow* window, int button, int action, int mode) -> void
        {});

        // set GLFW callbacks
        glfwSetCursorPosCallback(m_Window.Handle, [](GLFWwindow* window, double xPos, double yPos) -> void
        {});

        // set GLFW callbacks
        glfwSetKeyCallback(m_Window.Handle,[](GLFWwindow* window, int key, int code, int action, int mode) -> void
        {
			if (Input::IsKeyPressed(Key::Escape))
			{
				auto app = (Lunar::Application *)glfwGetWindowUserPointer(window);
				app->Shutdown();
			}
		});

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

			// Update every layer
            for (auto& layer : m_LayerStack) {
                layer->OnUpdate(m_TimeStep);
            }
            // Swap GLFW Buffer
			glfwSwapBuffers(m_Window.Handle);

            // update time past (for animation)
            float time = Application::GetTime();
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

	const WindowData& Application::GetWindowData() const
	{
		return m_Window;
	}
}
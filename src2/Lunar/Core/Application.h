//
// Created by Minkyeu Kim on 7/21/23.
//

#ifndef SCOOP_APPLICATION_H
#define SCOOP_APPLICATION_H

#include "Layer.h"

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <functional> // for lamda callback

#include <GL/glew.h>

struct GLFWwindow; // forward decl

// Structure Reference
// https://github.com/StudioCherno/Walnut/blob/3b8e414fdecfc6c8b58816106fe8d912bd172e31/Walnut/src/Walnut/Application.h

namespace Lunar {

	// NOTE: Application must be single instance.
	struct ApplicationSpecification
	{
		const char* Name = "Lunar App"; // default val
		uint32_t Width = 1600; // default val
		uint32_t Height = 900; // default val
	};

	struct WindowData
	{
		GLFWwindow* Handle = nullptr;
		GLint BufferWidth = 0;
		GLint BufferHeight = 0;
	};

	class Application
	{
	private:
		ApplicationSpecification m_Specification;
		WindowData m_Window;

		bool m_Running = false;
		float m_TimeStep = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		// https://www.youtube.com/watch?v=_Kj6BSfM6P4&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=13&pp=iAQB
		std::vector<std::shared_ptr<Layer>> m_LayerStack;

	private: // Helper function
		void Init() noexcept; // initialize GLFW context
		void Shutdown() noexcept; // clear LayerStack

	public:
		explicit Application(ApplicationSpecification  appSpec = ApplicationSpecification()) noexcept;
		~Application() noexcept;

		static Application& Get() noexcept;
        GLFWwindow* GetWindowHandle() const noexcept;

		void Run() noexcept;
        void Close() noexcept;
        static float GetTime() noexcept;

		// WHAT IS Layer?? WHY?
		template<typename T>
		void PushLayer()
		{
			// check if T is derived from Layer class at compile time.
			static_assert(std::is_base_of_v<Layer, T>, "Pushed type is not subclass of Layer!");
			m_LayerStack.push_back(std::make_shared<T>());
            m_LayerStack.back()->OnAttach();
		};
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv) noexcept;
}



#endif //SCOOP_APPLICATION_H
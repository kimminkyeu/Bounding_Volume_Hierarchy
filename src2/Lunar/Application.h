//
// Created by Minkyeu Kim on 7/21/23.
//

#ifndef SCOOP_APPLICATION_H
#define SCOOP_APPLICATION_H

#include "Layer.h"

#include <string>
#include <vector>
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
		std::string Name = "Lunar App"; // default val
		uint32_t Width = 1600; // default val
		uint32_t Height = 900; // default val
	};

	class Application
	{
	private:
		ApplicationSpecification m_Specification;
		GLFWwindow* m_WindowHandle = nullptr;
		bool m_Running = false;

		float m_TimeStep = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		// 왜 Stack으로 했는지?
		// A 화면에서 B화면 레이어를 띄운 후 B를 끄면 A가 꺼지는 브라우저 탐험 구조와 동일?
		std::vector<std::shared_ptr<Layer>> m_LayerStack;

	public:
		Application(const ApplicationSpecification& appSpec = ApplicationSpecification());
		~Application();

		//
		static Application& Get();

		//
		void Run(); // run app

		//
		template<typename T>
		void PushLayer()
		{
			// check if T is derived from Layer class at compile time.
			static_assert(std::is_base_of_v<Layer, T>, "Pushed type is not subclass of Layer!");
			// about emplace_back:  오히려 사용 하지 말라...?
			// https://modoocode.com/326
//			m_LayerStack.emplace_back(std::make_shared<T>());
			m_LayerStack.push_back(std::make_shared<T>());
		};

		void Close();

		float GetTime();

		GLFWwindow* GetWindowHandle() const
		{ return m_WindowHandle; }

	private: // Helper function
		void Init();
		void Shutdown();

	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);
}



#endif //SCOOP_APPLICATION_H

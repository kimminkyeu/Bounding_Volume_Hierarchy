//
// Created by USER on 2023-07-19.
//

#include "Window.h"

Window::Window()
	: m_Width(800), m_Height(600), m_BufferWidth(0), m_BufferHeight(0), m_MainWindowPtr(nullptr)
{}

[[maybe_unused]] Window::Window(GLint windowWidth, GLint windowHeight)
	: m_Width(windowWidth), m_Height(windowHeight), m_BufferWidth(0), m_BufferHeight(0), m_MainWindowPtr(nullptr)
{}

Window::~Window()
{
	glfwDestroyWindow(m_MainWindowPtr);
	glfwTerminate();
}

int Window::Initialize()
{
	if (!glfwInit()) // Init GLFW
	{
		std::cout << "GLFW Init failed\n";
		glfwTerminate();
		return (-1);
	}
	// Setup GLFW window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL major version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL minor version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile = No Backwards Compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Allow forward compatibility
	m_MainWindowPtr = glfwCreateWindow(m_Width, m_Height, "Test Window", nullptr, nullptr);
	if (!m_MainWindowPtr)
	{
		std::cout << "GLFW window creation failed\n";
		glfwTerminate();
		return (-1);
	}
	// Get Buffer size information (data for window rendering)
	glfwGetFramebufferSize(m_MainWindowPtr, &m_BufferWidth, &m_BufferHeight);
	glfwMakeContextCurrent(m_MainWindowPtr); // Set context for GLEW to use (그럼 multiple context 존재? 여러 화면?)
	glewExperimental = GL_TRUE; // Allow modern extension features

	if (glewInit() != GLEW_OK) // Check init state
	{
		std::cout << "GLEW init failed\n";
		glfwDestroyWindow(m_MainWindowPtr);
		glfwTerminate();
		return (-1);
	}
	glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
	glViewport(0, 0, m_BufferWidth, m_BufferHeight); // Setup Viewport size (OpenGL functionality)
	return (0);
}

GLint Window::getBufferWidth() const
{ return m_BufferWidth; }

GLint Window::getBufferHeight() const
{ return m_BufferHeight; }

void Window::swapBuffers() const
{ glfwSwapBuffers(m_MainWindowPtr); }

bool Window::shouldClose() const
{ return glfwWindowShouldClose(m_MainWindowPtr); }

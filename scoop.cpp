
#include <iostream>

#include <GL/glew.h>
// #include "GL/glew.h"
#include <GLFW/glfw3.h>
// #include "GLFW/glfw3.h"

// window dimensions
const GLint WIDTH = 800;
const GLint HEIGHT = 600;

int main()
{
	// Init GLFW
	if (!glfwInit())
	{
		std::cout << "GLFW Init failed\n";
		glfwTerminate();
		return (1);
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backwards Compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// nullptr != 0 (값 0과 구분)
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", nullptr, nullptr);
	if (!mainWindow)
	{
		std::cout << "GLFW window creation failed\n";
		glfwTerminate();
		return (1);
	}

	// Get Buffer size information (data for window rendering)
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use (그럼 multiple context 존재? 여러 화면?)
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW init failed\n";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return (1);
	}

	// Setup Viewport size (OpenGL functionality)
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Loop until mainWindow closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events.
		glfwPollEvents(); // send endpoint repeatedly.

		// Clear window
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// 2 Buffer (그리는 것과 그릴 예정인 것, 이 두개를 이용한 버퍼링)
		// 3개를 쓰기도 하지만, 여기선 2개만 사용.
		glfwSwapBuffers(mainWindow);
	}



	return (0);
}




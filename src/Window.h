//
// Created by USER on 2023-07-19.
//

#ifndef SCOOP_WINDOW_H
#define SCOOP_WINDOW_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Window
{
public:
	Window();
	[[maybe_unused]] Window(GLint windowWidth, GLint windowHeight);
	~Window();
	int Initialize();
	[[nodiscard]] GLint getBufferWidth() const;
	[[nodiscard]] GLint getBufferHeight() const;
	[[nodiscard]] bool shouldClose() const;

	bool* getKeys();
	GLfloat getMouseXChange();
	GLfloat getMouseYChange();

	// glfw의 2 Buffer Swapping (그리는 것과 그릴 예정인 것, 이 두개를 이용한 버퍼링)
	//	https://www.glfw.org/docs/3.3/quick.html#quick_swap_buffers
	void swapBuffers() const;

	// static function to use at Camera
	static glm::vec2 getMousePosition();


private:
	GLFWwindow* m_MainWindowPtr;
	GLint m_Width, m_Height;
	GLint m_BufferWidth, m_BufferHeight;

	bool m_Keys[1024] = {false,};
	void createCallbacks(); // input callback
	GLfloat m_LastMouseX;
	GLfloat m_LastMouseY;
	GLfloat m_MouseXChange;
	GLfloat m_MouseYChange;
	bool m_MouseFirstMoved;

	// used in GLFW callback.
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouseMove(GLFWwindow* window, double xPos, double yPos);
	static void handleMouseClick(GLFWwindow* window, int button, int action, int mode);
};


#endif //SCOOP_WINDOW_H

//
// Created by USER on 2023-07-19.
//

#ifndef SCOOP_WINDOW_H
#define SCOOP_WINDOW_H

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	[[maybe_unused]] Window(GLint windowWidth, GLint windowHeight);
	~Window();
	int Initialize();
	[[nodiscard]] GLint getBufferWidth() const;
	[[nodiscard]] GLint getBufferHeight() const;
	// glfw의 2 Buffer Swapping (그리는 것과 그릴 예정인 것, 이 두개를 이용한 버퍼링)
	void swapBuffers() const;
	[[nodiscard]] bool shouldClose() const;

private:
	GLFWwindow* m_MainWindowPtr;
	GLint m_Width, m_Height;
	GLint m_BufferWidth, m_BufferHeight;
};


#endif //SCOOP_WINDOW_H

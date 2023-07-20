//

#include "Window.h"

Window::Window()
	: m_Width(800), m_Height(600), m_BufferWidth(0), m_BufferHeight(0), m_MainWindowPtr(nullptr), m_MouseXChange(0.0f), m_MouseYChange(0.0f)
{}

Window::Window(GLint windowWidth, GLint windowHeight)
	: m_Width(windowWidth), m_Height(windowHeight), m_BufferWidth(0), m_BufferHeight(0), m_MainWindowPtr(nullptr), m_MouseXChange(0.0f), m_MouseYChange(0.0f)
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

	// set the current context
	glfwMakeContextCurrent(m_MainWindowPtr); // Set context for GLEW to use (그럼 multiple context 존재? 여러 화면?)

	// handle key + mouse input
	this->createCallbacks();
	// 커서 화면에서 안보이게 설정하기
//	glfwSetInputMode(m_MainWindowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 아래 설정은 Swap Buffer에 대한 중요 설정임.
	// 상제한 내용은 링크 참조: https://www.glfw.org/docs/3.3/quick.html#quick_swap_buffers
	glfwSwapInterval(1); // 무조건 업데이트 하지 않고 1frame 기다렸다가 update.

	glewExperimental = GL_TRUE; // Allow modern extension features

	if (glewInit() != GLEW_OK) // Check init state
	{
		std::cout << "GLEW init failed\n";
		glfwDestroyWindow(m_MainWindowPtr);
		glfwTerminate();
		return (-1);
	}
	glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)

	// create Viewport
	glViewport(0, 0, m_BufferWidth, m_BufferHeight); // Setup Viewport size (OpenGL functionality)

	// 이 객체가 glfw를 호출한다고 등록. (그닥 좋은 방식은 아닌듯.) (그닥 좋은 방식은 아닌듯...)
	glfwSetWindowUserPointer(m_MainWindowPtr, this);
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

void Window::createCallbacks()
{
	glfwSetKeyCallback(m_MainWindowPtr, this->handleKeys);
	glfwSetCursorPosCallback(m_MainWindowPtr, this->handleMouseMove);
	glfwSetMouseButtonCallback(m_MainWindowPtr, this->handleMouseClick);
}

// static function.
void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->m_Keys[key] = true;
			std::cout << "Pressed " << key << "\n";
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->m_Keys[key] = false;
			std::cout << "Released " << key << "\n";
		}
	}
}

void Window::handleMouseMove(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->m_MouseFirstMoved)
	{
		std::cout << "Mouse First Move\n";
		theWindow->m_LastMouseX = xPos;
		theWindow->m_LastMouseY = yPos;
		theWindow->m_MouseFirstMoved = false;
	}
	theWindow->m_MouseXChange = xPos - theWindow->m_LastMouseX;
	theWindow->m_MouseYChange = theWindow->m_LastMouseY - yPos;
	theWindow->m_LastMouseX = xPos;
	theWindow->m_LastMouseY = yPos;
//	std::cout << "x:" << std::fixed << std::setprecision(3) << theWindow->m_MouseXChange;
//	std::cout << " y:" << std::fixed << std::setprecision(3) << theWindow->m_MouseYChange << "\n";
}

void Window::handleMouseClick(GLFWwindow* window, int button, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		std::cout << "Pressed " << button << "\n";
	}
	else if (action == GLFW_RELEASE)
	{
		std::cout << "Released " << button << "\n";
	}
}

bool* Window::getKeys()
{ return m_Keys; }

GLfloat Window::getMouseXChange()
{
	GLfloat theChange = m_MouseXChange;
	m_MouseXChange = 0.0f;
	return theChange;
}

GLfloat Window::getMouseYChange()
{
	GLfloat theChange = m_MouseYChange;
	m_MouseYChange = 0.0f;
	return theChange;
}
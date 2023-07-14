#include <iostream>
#include <cstring>
#include <cassert>
#include <string>
#include <fstream> // for ifstream
#include <sstream> // for stringstream
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// window dimensions
const GLint WIDTH = 800;
const GLint HEIGHT = 600;

// VAO(Vertex Array Object) : 1개 vertex에 들어있는 데이터 명세
// VBO(Vertex Buffer Object) : vertex 자체
GLuint VAO, VBO, shader;

void create_triangle()
{
	GLfloat verticies[9] = {
			-1.0f, -1.0f, 0.0f,	// v0. x y z
			1.0f, -1.0f, 0.0f, 	// v1. x y z
			0.0f, 1.0f, 0.0f 	// v2. x y z
	};
	// (1) create VAO
	glGenVertexArrays(1, &VAO); // store id to VAO variable
	glBindVertexArray(VAO);

	// (2) create VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

	// ! what is stride?? what is index?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	// unbind data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void add_shader(GLuint theProgram, const std::string& shader_code, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLint len = shader_code.length();
	glShaderSource(theShader, 1, (const GLchar *const *)shader_code.c_str(), &len);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0, };
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(eLog), nullptr, eLog);
		std::cout << eLog << "\n";
		return ;
	}
	glAttachShader(theProgram, theShader);
}

// read file.
std::string convert_shader_file_to_string(const std::string& path)
{
	std::ifstream t(path);
	if (t.fail()) {
		return {""};
	}
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

void compile_shaders()
{
	shader = glCreateProgram();

	if (!shader)
	{
		std::cout << "Shader program creation failure\n";
		return ;
	}
	std::string vertex_shader = convert_shader_file_to_string("../src/vertex_shader.glsl");
	std::string fragment_shader = convert_shader_file_to_string("../src/fragment_shader.glsl");
	if (vertex_shader.empty() || fragment_shader.empty())
	{
		assert(false && "shader file to string error");
		// ...
	}
	else
	{
		std::cout << vertex_shader << "\n";

		add_shader(shader, vertex_shader, GL_VERTEX_SHADER);
		add_shader(shader, fragment_shader, GL_FRAGMENT_SHADER);
		GLint result = 0;
		GLchar eLog[1024] = {0,}; // shader 디버깅은 어렵기 떄문에 이런 방식으로 처리.

		glLinkProgram(shader);
		glGetProgramiv(shader, GL_LINK_STATUS, &result);
		if (!result) {
			memset(eLog, 0, sizeof(eLog));
			glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
			std::cout << eLog << "\n";
		}

		glValidateProgram(shader);
		glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
		if (!result) {
			memset(eLog, 0, sizeof(eLog));
			glGetProgramInfoLog(shader, sizeof(eLog), nullptr, eLog);
			std::cout << eLog << "\n";
		}
	}
}

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

	//
	create_triangle();
	compile_shaders();

	// Loop until mainWindow closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events.
		glfwPollEvents(); // send endpoint repeatedly.

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ------------------------------------
		glUseProgram(shader); // let GPU use the given shader program

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0); // unbind
		// ------------------------------------

		// 2 Buffer (그리는 것과 그릴 예정인 것, 이 두개를 이용한 버퍼링)
		// 3개를 쓰기도 하지만, 여기선 2개만 사용.
		glfwSwapBuffers(mainWindow);
	}
	return (0);
}




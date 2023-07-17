#include <iostream>
#include <cstring>
#include <cassert>

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

// Hello, Triangle example.
//https://heinleinsgame.tistory.com/7
#ifndef PROJECT_ROOT_DIR // cmake에서 설정해주도록 함.
# define PROJECT_ROOT_DIR "../" // ??
#endif


// window dimensions
constexpr GLint WIDTH = 800;
constexpr GLint HEIGHT = 600;
constexpr float toRadians = 3.14159265f / 100.0f;
float curAngle = 0;

std::vector< Mesh* > meshList;

// VAO(Vertex Array Object) : 1개 vertex에 들어있는 데이터 명세
// VBO(Vertex Buffer Object) : vertex 자체
// IBO(Indexed Buffer Object) : vertex 묶음 (면).

GLuint shaderProgram;
GLint MODEL_LOCATION, PROJECTION_LOCATION;

void add_shader(GLuint theProgram, const std::string& shader_code, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);
	const GLint len = shader_code.length();
	const char* src = shader_code.c_str();
	glShaderSource(theShader, 1, &src, &len);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0, };
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, sizeof(eLog), nullptr, eLog);
		std::cout << "[glGetShaderInfoLog]" << eLog << "\n";
		return ;
	}
	glAttachShader(theProgram, theShader);
}

void compile_shaders()
{
	std::string project_root_dir = PROJECT_ROOT_DIR;
	std::string vertex_shader_path = project_root_dir + std::string("/src/vertex_shader.glsl");
	std::string fragment_shader_path = project_root_dir + std::string("/src/fragment_shader.glsl");
	std::string vertex_shader_code = convert_shader_file_to_string(vertex_shader_path);
	std::string fragment_shader_code = convert_shader_file_to_string(fragment_shader_path);

	if (vertex_shader_code.empty() || fragment_shader_code.empty())
	{
		assert(false && "shader file to string error");
		// ...
	}
	else
	{
		shaderProgram = glCreateProgram();
		if (!shaderProgram)
		{
			std::cout << "Shader program creation failure\n";
			return ;
		}
		add_shader(shaderProgram, vertex_shader_code, GL_VERTEX_SHADER);
		add_shader(shaderProgram, fragment_shader_code, GL_FRAGMENT_SHADER);
		GLint result = 0;
		GLchar eLog[1024] = {0,}; // shader 디버깅은 어렵기 떄문에 이런 방식으로 처리.

		glLinkProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
		if (!result) {
			memset(eLog, 0, sizeof(eLog));
			glGetProgramInfoLog(shaderProgram, sizeof(eLog), nullptr, eLog);
			std::cout << "[glGetProgramInfoLog: linking shader" << eLog << "\n";
		}

		glValidateProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &result);
		if (!result) {
			memset(eLog, 0, sizeof(eLog));
			glGetProgramInfoLog(shaderProgram, sizeof(eLog), nullptr, eLog);
			std::cout << "[glGetProgramInfoLog: validation]" << eLog << "\n";
		}
	}
	// **************************************************************************
	// shader 프로그램의 uniform 변수중 이름이 "matrix"인 친구를 찾아 그 location를 반환.
	MODEL_LOCATION = glGetUniformLocation(shaderProgram, "model");
	PROJECTION_LOCATION = glGetUniformLocation(shaderProgram, "projection");
	// **************************************************************************
}

void create_triangle()
{
	unsigned int indices[] = {
			0, 3, 1,
			1, 3, 2,
			2, 3, 0,
			0, 1, 2
	};

	GLfloat verticies[] = {
			-1.0f, -1.0f, 0.0f,	// v0. x y z
			0.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 	// v1. x y z
			0.0f, 1.0f, 0.0f 	// v2. x y z
	};

	Mesh *obj1_ptr = new Mesh();
	obj1_ptr->CreateMesh(verticies, indices, 12, 12);
	meshList.push_back(obj1_ptr);

	Mesh *obj2_ptr = new Mesh();
	obj1_ptr->CreateMesh(verticies, indices, 12, 12);
	meshList.push_back(obj1_ptr);
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

	// 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
	glEnable(GL_DEPTH_TEST);

	// Setup Viewport size (OpenGL functionality)
	glViewport(0, 0, bufferWidth, bufferHeight);

	create_triangle();
	compile_shaders();

	glm::mat4 projectionMatrix = glm::perspective(45.0f, (GLfloat)bufferWidth/(GLfloat)bufferHeight, 0.1f, 100.0f);

	// Loop until mainWindow closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events.
		glfwPollEvents(); // send endpoint repeatedly.

		curAngle += 0.5f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ------------------------------------
		glUseProgram(shaderProgram); // let GPU use the given shader program

		glm::mat4 matrix(1.0f); // init unit matrix
		glUniformMatrix4fv(PROJECTION_LOCATION, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		// [t] * [s] * [pos]d
		matrix = glm::translate(matrix, glm::vec3(1.0f, 0.0f, -2.5f));
		matrix = glm::scale(matrix, glm::vec3(0.4f, 0.4, 0.4f));
		glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, glm::value_ptr(matrix));
		meshList[0]->RenderMesh();

		matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, glm::vec3(-1.0f, 0.0f, -2.5f));
		matrix = glm::scale(matrix, glm::vec3(0.4f, 0.4, 0.4f));
		glUniformMatrix4fv(MODEL_LOCATION, 1, GL_FALSE, glm::value_ptr(matrix));
		meshList[1]->RenderMesh();

		glUseProgram(0); // unbind
		// ------------------------------------
		// 2 Buffer (그리는 것과 그릴 예정인 것, 이 두개를 이용한 버퍼링)
		glfwSwapBuffers(mainWindow);
	}
	for (auto &itr: meshList) {
		delete itr;
	}
	glfwTerminate();
	return (0);
}




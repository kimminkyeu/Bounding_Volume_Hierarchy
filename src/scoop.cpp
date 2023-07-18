#include <iostream>
#include <cstring>
#include <cassert>

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h" // Mesh Class
#include "ShaderProgram.h" // ShaderProgram Class

#ifndef PROJECT_ROOT_DIR // cmake에서 설정해주도록 함.
# define PROJECT_ROOT_DIR "../" // ??
#endif


// window dimensions
constexpr GLint WIDTH = 800;
constexpr GLint HEIGHT = 600;
constexpr float toRadians = 3.14159265f / 100.0f;
float curAngle = 0;

std::vector< Mesh* > meshList;
std::vector< ShaderProgram* > shaderList;

// VAO(Vertex Array Object) : 1개 vertex에 들어있는 데이터 명세
// VBO(Vertex Buffer Object) : vertex 자체
// IBO(Indexed Buffer Object) : vertex 묶음 (면).

// 나중에 Triangle Object로 변경할 것.
void createObject()
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

void createShaders()
{
	ShaderProgram* shader1 = new ShaderProgram();
	shader1->createFromString();
	shaderList.push_back(shader1);
}

int main()
{
	if (!glfwInit()) // Init GLFW
	{
		std::cout << "GLFW Init failed\n";
		glfwTerminate();
		return (1);
	}
	// Setup GLFW window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL major version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // OpenGL minor version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile = No Backwards Compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Allow forward compatibility
	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", nullptr, nullptr);
	if (!mainWindow)
	{
		std::cout << "GLFW window creation failed\n";
		glfwTerminate();
		return (1);
	}
	int bufferWidth, bufferHeight; // Get Buffer size information (data for window rendering)
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
	glfwMakeContextCurrent(mainWindow); // Set context for GLEW to use (그럼 multiple context 존재? 여러 화면?)
	glewExperimental = GL_TRUE; // Allow modern extension features

	if (glewInit() != GLEW_OK) // Check init state
	{
		std::cout << "GLEW init failed\n";
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return (1);
	}
	glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
	glViewport(0, 0, bufferWidth, bufferHeight); // Setup Viewport size (OpenGL functionality)

	// ---------------------------------------------

	createObject();
	createShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;

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
		glUseProgram(shaderList[0]->)
		uniformModel = shaderList[0]->getUniformModelLocation();
		uniformProjection = shaderList[0]->getUniformProjectionLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix)); // GPU 변수 location에 값 대입.


		// [t] * [s] * [pos]d
		glm::mat4 matrix(1.0f); // init unit matrix
		matrix = glm::translate(matrix, glm::vec3(1.0f, 0.0f, -2.5f));
		matrix = glm::scale(matrix, glm::vec3(0.4f, 0.4, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrix)); // GPU 변수 location에 값 대입.
		meshList[0]->RenderMesh();

		matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, glm::vec3(-1.0f, 0.0f, -2.5f));
		matrix = glm::scale(matrix, glm::vec3(0.4f, 0.4, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrix)); // GPU 변수 location에 값 대입.
		meshList[1]->RenderMesh();


		glUseProgram(0); // unbind
		glfwSwapBuffers(mainWindow); // 2 Buffer (그리는 것과 그릴 예정인 것, 이 두개를 이용한 버퍼링)
	}


	for (auto &itr: meshList) {
		delete itr;
	}
	glfwTerminate();
	return (0);
}




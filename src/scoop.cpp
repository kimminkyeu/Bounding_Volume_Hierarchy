#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h" // GLFW wrapper
#include "Mesh.h" // Mesh Class
#include "ShaderProgram.h" // ShaderProgram Class

#ifndef PROJECT_ROOT_DIR // cmake에서 설정해주도록 함.
# define PROJECT_ROOT_DIR "../" // ??
#endif

std::vector< Mesh* > meshList;
std::vector< ShaderProgram* > shaderProgramList;

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
	obj2_ptr->CreateMesh(verticies, indices, 12, 12);
	meshList.push_back(obj2_ptr);
}

int main()
{
	Window mainWindow;
	mainWindow.Initialize();

	createObject();

	// ------------------------------------
	auto* shaderProc = new ShaderProgram();
	std::string vShaderPath = std::string(PROJECT_ROOT_DIR) + std::string("/src/shader/vertex_shader.glsl");
	shaderProc->attachShader(vShaderPath, GL_VERTEX_SHADER);
	std::string fShaderPath = std::string(PROJECT_ROOT_DIR) + std::string("/src/shader/fragment_shader.glsl");
	shaderProc->attachShader(fShaderPath, GL_FRAGMENT_SHADER);
	shaderProc->linkToGPU();
	shaderProgramList.push_back(shaderProc);
	// ------------------------------------

	glm::mat4 projectionMatrix = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth()/(GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Loop until mainWindow closed
	while (!mainWindow.shouldClose())
	{
		// Get + Handle user input events.
		glfwPollEvents(); // send endpoint repeatedly.

		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set ShaderProgram
		// WARN: useProgram을 먼저 해야 아래 setUniformProjection이 작동함!
		glUseProgram(shaderProgramList[0]->getProgramID());

		// set projection
		shaderProgramList[0]->setUniformProjection(glm::value_ptr(projectionMatrix));

		// first mesh
		glm::mat4 matrix(1.0f); // init unit matrix
		matrix = glm::translate(matrix, glm::vec3(1.0f, 0.0f, -2.5f));
		matrix = glm::scale(matrix, glm::vec3(0.4f, 0.4, 0.4f));
		shaderProgramList[0]->setUniformModel(glm::value_ptr(matrix));
		meshList[0]->RenderMesh();

		// second mesh
		matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, glm::vec3(-1.0f, 0.0f, -2.5f));
		matrix = glm::scale(matrix, glm::vec3(0.4f, 0.4, 0.4f));
		shaderProgramList[0]->setUniformModel(glm::value_ptr(matrix));
		meshList[1]->RenderMesh();

		// unset ShaderProgram
		glUseProgram(0);
		// Swap GLFW Buffer
		mainWindow.swapBuffers();
	}

	for (auto &itr: meshList) {
		delete itr;
	}
	return (0);
}




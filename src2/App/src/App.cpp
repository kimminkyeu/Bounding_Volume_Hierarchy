
#include <glm/glm.hpp>

#include "Lunar/Core/Application.h"
#include "Lunar/Core/EntryPoint.h" // main code here
#include "Lunar/Shader/ShaderProgram.h"
#include "Lunar/Mesh/Mesh.h"

class ExampleLayer : public Lunar::Layer
{
private:
	Lunar::ShaderProgram m_ShaderProgram;
	std::vector<std::unique_ptr<Lunar::Mesh>> m_MeshList;
	glm::mat4 m_ProjectionMatrix; // TODO: MOVE THIS TO CAMERA CLASS!!

	GLuint VAO = 0; // TODO: Temp!
	GLuint VBO = 0;

public:
	// called once pushed to m_LayerStack
	void OnAttach() override
	{
		LOG_TRACE("Layer has been attached");
		// load shader
		m_ShaderProgram = Lunar::ShaderProgram(
	   "/src2/App/src/shaders/vertex_shader.glsl",
	 "/src2/App/src/shaders/fragment_shader.glsl");

		GLfloat verticies[9] = {
				-1.0f, -1.0f, 0.0f,	// v0. x y z
				1.0f, -1.0f, 0.0f, 	// v1. x y z
				0.0f, 1.0f, 0.0f 	// v2. x y z
		};
		glGenVertexArrays(1, &VAO); // store id to VAO variable
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);



		// TODO: 일단 mesh를 쓰지 마. 그냥 삼각형만 띄워보자.
		/*
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
		m_MeshList.push_back(std::make_unique<Lunar::Mesh>(verticies, indices, 12, 12));
		*/

//		const auto& app = Lunar::Application::Get();
//		const auto& windowData = app.getWindowData();
//		m_ProjectionMatrix = glm::perspective(45.0f, (GLfloat)windowData.BufferWidth/(GLfloat)windowData.BufferHeight, 0.1f, 100.0f);
	}

	// called once popped from m_LayerStack
	void OnDetach() override
	{
		LOG_TRACE("Layer has been detached");
		for (auto &mesh : m_MeshList)
			mesh->ClearMesh(); // delete mesh buffer (VAO VBO IBO)
	}

	// called every render loop
	void OnUpdate(float ts) override
	{
		// Set shader program for single frame parallel rendering.
		glUseProgram(m_ShaderProgram.getProgramID());

		// Set Projection (TODO: move this to Camera class)
//		glm::mat4 test(1.0f); // TODO: move to CAMERA!
//		m_ShaderProgram.setUniformProjection(glm::value_ptr(test));

//		glm::mat4 view(1.0f); // TODO: move to CAMERA!
//		m_ShaderProgram.setUniformView(glm::value_ptr(view));

		// Set Model
//		glm::mat4 model(1.0f); // init unit matrix
//		model = glm::translate(model, glm::vec3(1.0f, 0.0f, -2.5f));
//		model = glm::scale(model, glm::vec3(0.4f, 0.4, 0.4f));
//		m_ShaderProgram.setUniformModel(glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);


		// Render each mesh
		// TODO: MOVE LOCATION MATRIX TO MESH CLASS.
//		for (auto& mesh : m_MeshList) {
//			mesh->RenderMesh();
//		}

		// unbind shader program
		glUseProgram(0);
	}

    void OnUIRender() override
    {
        // ....
    }
};

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec {"Scoop", 600, 600 };

	auto* app = new Lunar::Application(spec);
    app->PushLayer<ExampleLayer>();
	return (app);
}
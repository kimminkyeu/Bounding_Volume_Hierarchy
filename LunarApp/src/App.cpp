
#include <glm/glm.hpp>

#include "Lunar/Core/Application.h"
#include "Lunar/Core/EntryPoint.h" // main code here
#include "Lunar/Shader/ShaderProgram.h"
#include "Lunar/Mesh/Mesh.h"

class ExampleLayer final : public Lunar::Layer
{
private:
	std::unique_ptr<Lunar::ShaderProgram> m_ShaderProgram;
	std::vector<std::unique_ptr<Lunar::Mesh>> m_MeshList;
	glm::mat4 m_ProjectionMatrix; // TODO: MOVE THIS TO CAMERA CLASS!!

public:
	ExampleLayer()
		: m_ProjectionMatrix(glm::mat4(1.0f))
	{
		this->_m_Name = "Example";
		LOG_TRACE("Layer [{0}] constructor called", _m_Name);
	}

	~ExampleLayer() override
	{
		LOG_TRACE("Layer [{0}] destructor called", _m_Name);
	}

	// called once pushed to m_LayerStack
	void OnAttach() override
	{
		LOG_TRACE("Layer [{0}] has been attached", _m_Name);

		// load shader // 아 복사 생성자 만들고 소멸자 호출되서 쉐이더가 터진거였네;;;
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

		// WARN:  VAO가 먼저 잡힌 뒤에 shader를 링크해야 한다.?? 그 이전에 하면 VAO bound error가 발생함...
		// https://stackoverflow.com/questions/54181078/opengl-3-3-mac-error-validating-program-validation-failed-no-vertex-array-ob
		m_ShaderProgram = std::make_unique<Lunar::ShaderProgram>(
				"LunarApp/src/shaders/vertex_shader.glsl",
				"LunarApp/src/shaders/fragment_shader.glsl");

		const auto& app = Lunar::Application::Get();
		const auto& windowData = app.getWindowData();
		m_ProjectionMatrix = glm::perspective(45.0f, (GLfloat)windowData.BufferWidth/(GLfloat)windowData.BufferHeight, 0.1f, 100.0f);
	}

	// called once popped from m_LayerStack
	void OnDetach() override
	{
		LOG_TRACE("Layer [{0}] has been detached", _m_Name);
		for (auto &mesh : m_MeshList)
			mesh->ClearMesh(); // delete mesh buffer (VAO VBO IBO)
	}

	// called every render loop
	void OnUpdate(float ts) override
	{
		// Set shader program for single frame parallel rendering.
		glUseProgram(m_ShaderProgram->getProgramID());

		// Set Projection (TODO: move this to Camera class)
		m_ShaderProgram->setUniformProjection(glm::value_ptr(m_ProjectionMatrix));

		glm::mat4 view(1.0f); // TODO: move to CAMERA!
		m_ShaderProgram->setUniformView(glm::value_ptr(view));

		// Set Model
		glm::mat4 model(1.0f); // init unit matrix
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4, 0.4f));
		m_ShaderProgram->setUniformModel(glm::value_ptr(model));

		// Render each mesh
		// TODO: MOVE LOCATION MATRIX TO MESH CLASS.
		for (auto& mesh : m_MeshList) {
			mesh->RenderMesh();
		}

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
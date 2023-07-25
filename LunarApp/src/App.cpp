
#include <glm/glm.hpp>

#include "Lunar/Core/Application.h"
#include "Lunar/Core/EntryPoint.h" // main code here
#include "Lunar/Shader/ShaderProgram.h"
#include "Lunar/Mesh/Mesh.h"
#include "Lunar/Camera/EditorCamera.h"
#include "Lunar/Texture/Texture.h"
#include "Lunar/Model/Model.h"
#include "Lunar/Light/Light.h"

class ExampleLayer final : public Lunar::Layer
{
private:
	std::unique_ptr<Lunar::ShaderProgram> m_ShaderProgram;
	std::vector<std::unique_ptr<Lunar::Mesh>> m_MeshList;
	Lunar::EditorCamera m_EditorCamera;
	Lunar::Texture m_BrickTexture {"LunarApp/assets/brick_wall.png"};
	Lunar::Model m_Model;
//	Lunar::Light m_MainLight {0.0f, 0.0f, 0.0f, 1.0f };

public:
	ExampleLayer()
	{
		this->_m_Name = "Example";
		const auto& app = Lunar::Application::Get();

		// ******* Init Camera *********
		auto width = app.getWindowData().BufferWidth;
		auto height = app.getWindowData().BufferHeight;
		auto aspectRatio = (float)width / (float)height;
		m_EditorCamera = Lunar::EditorCamera(45.0f, aspectRatio, 0.1f, 100.0f);
		// *****************************

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
				0, 1, 2,
		};
		GLfloat verticies[] = {
		//          X          Y          Z          U         V
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 1.0f, 0.5f, 0.0f,
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		};
		m_MeshList.push_back(std::make_unique<Lunar::Mesh>(verticies, indices, 20, 12));
		m_BrickTexture.LoadTextureRGBA();
//		m_Model.LoadModel("LunarApp/assets/42.obj");
//		m_Model.LoadModel("LunarApp/assets/42.obj");
//		m_Model.LoadModel("LunarApp/assets/teapot2.obj");



		// TODO: 왜 모델을 로드하기 전에 shader를 링크하면 에러가 날까? (VAO 없다 오류)
		// TODO: VAO가 모델에 속해있는데, 모델이 먼저 안생기면 VAO가 없어서 터지는 걸까?

		// WARN:  VAO가 먼저 잡힌 뒤에 shader를 링크해야 한다.?? 그 이전에 하면 VAO bound error가 발생함...
		// https://stackoverflow.com/questions/54181078/opengl-3-3-mac-error-validating-program-validation-failed-no-vertex-array-ob
		m_ShaderProgram = std::make_unique<Lunar::ShaderProgram>(
				"LunarApp/src/shaders/vertex_shader.glsl",
				"LunarApp/src/shaders/fragment_shader.glsl");

		const auto& app = Lunar::Application::Get();
		const auto& windowData = app.getWindowData();

	}

	// called every render loop
	void OnUpdate(float ts) override
	{
		// Clear window to black.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// 0. Set shader program for single frame parallel rendering.

		glUseProgram(m_ShaderProgram->GetProgramID());

		// 1. update camera
		m_EditorCamera.OnUpdate(ts);

		// Set View, Projection Matrix (from Editor Camera)
		m_ShaderProgram->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
		m_ShaderProgram->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
		glm::mat4 model(1.0f); // init unit matrix
		m_ShaderProgram->SetUniformModel(glm::value_ptr(model));

//		m_MainLight.UseLight(m_ShaderProgram->GetUniformAmbientIntensityLocation(), m_ShaderProgram->GetUniformAmbientColorLocation());

		// Render each mesh
		m_BrickTexture.UseTexture(); // 모든 객체가 이 텍스쳐를 사용.
//		m_Model.RenderModel();
		// TODO: MOVE LOCATION MATRIX TO MESH CLASS.
		for (auto& mesh : m_MeshList) {
			mesh->RenderMesh();
		}
		// ********************************************************************

		// unbind shader program
		glUseProgram(0);
	}

    void OnUIRender() override
    {
        // ....
    }

	// called once popped from m_LayerStack
	void OnDetach() override
	{
		LOG_TRACE("Layer [{0}] has been detached", _m_Name);
		for (auto &mesh : m_MeshList)
			mesh->ClearMesh(); // delete mesh buffer (VAO VBO IBO)
	}

	Lunar::EditorCamera* GetActiveCameraPtr()
	{ return &m_EditorCamera; }
};

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec {"Scoop", 1200, 800 };

	auto* app = new Lunar::Application(spec);
    app->PushLayer<ExampleLayer>();
	return (app);
}

#include <glm/glm.hpp>

#include "Lunar/Camera/EditorCamera.h"
#include "Lunar/Core/Application.h"
#include "Lunar/Core/EntryPoint.h"// main code here
#include "Lunar/FrameBuffer/FrameBuffer.h"
#include "Lunar/Light/Light.h"
#include "Lunar/Material/Material.h"
#include "Lunar/Mesh/Mesh.h"
#include "Lunar/Model/Model.h"
#include "Lunar/Shader/Shader.h"
#include "Lunar/Texture/Texture.h"

#include "LunarApp/src/ShaderController.h"
#include "LunarApp/src/shaders/Shaded/ShadedShader.h"
#include "LunarApp/src/shaders/Explosion/ExplosionShader.h"
#include "LunarApp/src/shaders/Phong/PhongShader.h"
#include "LunarApp/src/shaders/Test/TestShader.h"
#include "LunarApp/src/shaders/Wireframe/WireframeShader.h"
#include "LunarApp/src/shaders/Normal/NormalShader.h"

class ExampleLayer final : public Lunar::Layer
{
private:
	ImVec2 m_Size; // NOTE: ImGUI Content Size, not screen size.

	ShaderController m_ShaderController; // group of shaders
	std::vector<std::shared_ptr<Lunar::Mesh>> m_MeshList;

	Lunar::EditorCamera m_EditorCamera;
	Lunar::Model m_Model;
	Lunar::Texture m_BrickTexture;
	Lunar::Light m_MainLight;
	Lunar::Material m_Material;

	// NOTE: Viewport Buffer (Only Rendering)
	Lunar::FrameBuffer m_FrameBuffer;

public:
	ExampleLayer()
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
		const auto& app = Lunar::Application::Get();
		auto width = app.GetWindowData().BufferWidth;
		auto height = app.GetWindowData().BufferHeight;

		m_FrameBuffer.Init((float)width, (float)height);


	// TODO: model load를 실패할 경우, vao가 없다. 따라서 shader load에서 validation error 발생. 이 경우 예외처리를 어떻게 하는게 좋을지?
	// 1. Create object
//		m_Model.LoadModel("LunarApp/assets/teapot2.obj");
		m_Model.LoadModel("LunarApp/assets/sphere.obj");
//		m_Model.LoadModel("LunarApp/assets/shaderBall.obj");

	// 2. Create Texture
		m_BrickTexture = Lunar::Texture("LunarApp/assets/brick.png");
		m_BrickTexture.LoadTexture();

	// 2. Create Material
		// ...

	// 3. Create Light
		m_MainLight = Lunar::Light( glm::vec3(2.0f, -1.0f, -1.0f), 0.4f, 0.4f, 0.4f );

	// 4. Init Camera
		auto aspectRatio = (float)width / (float)height;
		m_EditorCamera = Lunar::EditorCamera(45.0f, aspectRatio, 0.1f, 100.0f);

	// 5. Load Shaders 		// TODO: move to shader loader class

		m_ShaderController.Add( new ShadedShader() ); // Blender edit mode style [WireFrame + Flat Shading]
		m_ShaderController.Add( new ExplosionShader() );
		m_ShaderController.Add( new PhongShader() );
		m_ShaderController.Add( new WireframeShader() );
		m_ShaderController.Add( new TestShader() );
		m_ShaderController.Add( new NormalShader() );

		// TODO: add more shaders...
		//        - wireframe
		//        - shaded (wireframe + phong)
		//        - Cartoon
		//        - Phong Render ( = Texture가 있을 때만 텍스쳐 입히기 )
		//        - Flat Render --> 이건 굳이 따로 쉐이더를 나눌 필요 없이, 기본에서 설정 가능?
		//        - Gouraud Render --> 이것도...?
	}

	// called every render loop
	void OnUpdate(float ts) override
	{
		m_EditorCamera.OnUpdate(ts); // 2. update camera geometry

		// 0. bind frame buffer ( = render target image )
		m_FrameBuffer.Bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 1. Unbind current frame buffer data.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		{
			m_ShaderController.BindCurrentShader();
			const auto shaderProcPtr = m_ShaderController.GetCurrentShaderPtr();

			shaderProcPtr->SetUniformEyePos(m_EditorCamera.GetPosition());
			shaderProcPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
			shaderProcPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
			glm::mat4 model(1.0f);// init unit matrix
			shaderProcPtr->SetUniformModel(glm::value_ptr(model));

			// 2. set material to shader
			m_Material.UseMaterial(*shaderProcPtr);
			// 3. set texture to shader
			//			m_BrickTexture.UseTexture();
			// 4. BindCurrentShader Light
			m_MainLight.UseLight(*shaderProcPtr);

			m_Model.RenderModel();
			m_ShaderController.UnbindCurrentShader();

			// NOTE: Render Twice on the same Frame Buffer. (To show Normal)
			// https://stackoverflow.com/questions/37580597/best-way-to-use-multiple-shaders
			// https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
//			m_ShaderController.SetCurrentShader("Normal");
//			m_Model.RenderModel();
//			m_ShaderController.UnbindCurrentShader();
		}
		m_FrameBuffer.Unbind(); // unbind Frame Buffer (render target)
	}

	// NOTE: this is ImGui Render function
    void OnUIRender() override
	{
		const auto currentShaderName = m_ShaderController.GetCurrentShaderPtr()->GetName();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("ViewPort");
			// https://uysalaltas.github.io/2022/01/09/OpenGL_Imgui.html
			if (ImGui::BeginMenu(currentShaderName.c_str()))
			{
				for (auto &itr : m_ShaderController.GetShaderMap())
				{
					if (ImGui::MenuItem(itr.first.c_str())) {
						m_ShaderController.SetCurrentShader(itr.first);
					}
				}
				ImGui::EndMenu();
			}
			{
				m_Size = ImGui::GetContentRegionAvail();
				// NOTE: put m_FrameBuffer's image data to ImGui Image
				ImGui::Image(
						reinterpret_cast<void *>(m_FrameBuffer.GetFrameTexture()),
						m_Size,
						ImVec2(0, 1), // NOTE: ??
						ImVec2(1, 0) // NOTE: ??
				);
				// NOTE: re-calculate camera for viewport height change...
				m_EditorCamera.OnResize(m_Size.x, m_Size.y);
				//				OnResize(m_Size.x, m_Size.y); // --> 이렇게 하면 비율이 터짐.
			}
			ImGui::End();
			ImGui::PopStyleVar();
		}
		{
			// Material // https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/WalnutApp.cpp
			ImGui::Begin("Control Menu");
			if (currentShaderName == "Phong")
			{
				ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_Material.m_AmbientColor));
				ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_Material.m_DiffuseColor));
				ImGui::ColorEdit3("Specular Color", glm::value_ptr(m_Material.m_SpecularColor));
				ImGui::DragFloat("Specular Exponent", &m_Material.m_SpecularExponent);
			}
			else if (currentShaderName == "Explosion")
			{
				auto* ptr = dynamic_cast<ExplosionShader *>(m_ShaderController.GetCurrentShaderPtr());
				if (ptr != nullptr) {
					ImGui::SliderFloat("Explosion Degree", &(ptr->m_Degree), 0.0f, 10.0f, "%.1f");
				}
			}
			else
			{
				// ...
			}
			ImGui::End();
		}
	}

	// called once popped from m_LayerStack
	void OnDetach() override
	{
		LOG_TRACE("Layer [{0}] has been detached", _m_Name);
		for (auto &mesh : m_MeshList)
			mesh->ClearMesh(); // delete mesh buffer (VAO VBO IBO)
	}

	void OnResize(float width, float height) override
	{
		m_FrameBuffer.RescaleFrameBuffer(width, height);
		m_EditorCamera.OnResize(width, height); // re-calculate camera
	}
};

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec {"Scoop", 1000, 1000 };
	auto* app = new Lunar::Application(spec);
    app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()-> void
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				app->Reboot();
			}
			if (ImGui::MenuItem("Open"))
			{
				app->Reboot();
			}
			if (ImGui::MenuItem("Export"))
			{
				app->Reboot();
			}
			if (ImGui::MenuItem("Exit"))
			{
				app->Shutdown(); // sets m_Running to false.
			}
			if (ImGui::MenuItem("Restart"))
			{
				app->Reboot();
			}
			ImGui::EndMenu();
		}
	});
	return (app);
}

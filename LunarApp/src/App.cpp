
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

#include "LunarApp/src/shaders/DisplayMode.h"
#include "LunarApp/src/shaders/Explosion/ExplosionShader.h"
#include "LunarApp/src/shaders/Phong/PhongShader.h"
#include "LunarApp/src/shaders/Test/TestShader.h"
#include "LunarApp/src/shaders/Cartoon/CartoonShader.h"
#include "LunarApp/src/shaders/DataVisualizer.h"
#include "LunarApp/src/AABB/AABB.h"
#include "LunarApp/src/TestObject/TestObject.h"


// AABB 구현 방식
// 1. AABB에 model matrix를 곱한다.
// 2. RenderLoop에서 AABB를 그리는 쉐이더를 따로 생성한다. // AABB debug shader
// 3. AABB는 Object의 VAO와 IBO 데이터를 가지고 형성해야 한다.
// 4. 한 Mesh에 대해, VAO 데이터 배열을 돌면서 x y z축에 대한 최대/ 최소값를 찾아내야 한다.

// assimp를 쓰면 VAO IBO 생성하고 데이터 넘기고 끝내기 때문에, 별도 배열을 가지고 있어야 한다.

class ExampleLayer final : public Lunar::Layer
{
private:
	ImVec2 m_Size; // NOTE: ImGUI Content Size, not screen size.
    Lunar::FrameBuffer m_FrameBuffer; // NOTE: Viewport Buffer (Only Rendering)
	DisplayMode m_DisplayMode; // main display mode
private:
	DataVisualizer m_DataVisualizer; // vertex, polygon, normal visualizer
	Lunar::EditorCamera m_EditorCamera;
	Lunar::Model m_Model;
	Lunar::Light m_MainLight;
	Lunar::Material m_Material;
	// Lunar::Texture m_BrickTexture;

	// -----------------------------------------------------------
	bool m_ShowMesh = true;
	int m_BBoxDebugDrawLevel = 0;
	TestObject m_TestObject; // NOTE: Temporary data For AABB Test
	AABBTree* m_AABB = nullptr;
	// -----------------------------------------------------------

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


	// 1. Create object
//		m_Model.LoadModel("LunarApp/assets/teapot2.obj");
		m_Model.LoadModel("LunarApp/assets/sphere.obj");
//		m_Model.LoadModel("LunarApp/assets/shaderBall.obj");

	// 2. Create Texture
//		m_BrickTexture = Lunar::Texture("LunarApp/assets/brick.png");
//		m_BrickTexture.LoadTexture();

	// 2. Create Material
		// ...

	// 3. Create Light
		m_MainLight = Lunar::Light( glm::vec3(2.0f, -1.0f, -1.0f), 0.4f, 0.4f, 0.4f );

	// 4. Init Camera
		auto aspectRatio = (float)width / (float)height;
		m_EditorCamera = Lunar::EditorCamera(45.0f, aspectRatio, 0.1f, 100.0f);
		const auto p = m_EditorCamera.GetPosition();
		LOG_INFO("camera pos x{0} y{1} z{2}", p.x, p.y, p.z);
		const auto l = m_EditorCamera.GetForwardDirection();
		LOG_INFO("camera forward x{0} y{1} z{2}", l.x, l.y, l.z);

		m_DisplayMode.Add( new ExplosionShader() );
		m_DisplayMode.Add( new PhongShader() );
		m_DisplayMode.Add( new TestShader() );
		m_DisplayMode.Add( new CartoonShader() );
        m_DataVisualizer.Init(); // init wireframe, normal, vertex (Shader)

		// ------ AABB TEST -------------------
//		m_AABB = new AABBTree(m_TestObject.m_Vertices, m_TestObject.m_Indices);
		m_AABB = new AABBTree(m_Model.vertices, m_Model.indices);
		// ------------------------------------
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
			glm::mat4 model(1.0f); // init unit matrix
			// ---------------- Main Object Render ------------------
			if (m_ShowMesh)
			{
				m_DisplayMode.BindCurrentShader();
				const auto shaderProcPtr = m_DisplayMode.GetCurrentShaderPtr();
				shaderProcPtr->SetUniformEyePos(m_EditorCamera.GetPosition());
				shaderProcPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
				shaderProcPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
				shaderProcPtr->SetUniformModel(glm::value_ptr(model));
				m_Material.UseMaterial(*shaderProcPtr);
				//			m_BrickTexture.UseTexture();
				m_MainLight.UseLight(*shaderProcPtr);
				//			m_TestObject.Render(GL_TRIANGLES);
				m_Model.RenderModel(GL_TRIANGLES);
				m_DisplayMode.UnbindCurrentShader();
			}
			// ----------------  Normal Render ------------------
			if (m_DataVisualizer.m_ShowNormal)
			{
				auto* normalShaderPtr = m_DataVisualizer.m_NormalShader;
				if (normalShaderPtr)
				{
					normalShaderPtr->Bind();
					normalShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
					normalShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
					normalShaderPtr->SetUniformModel(glm::value_ptr(model));
//					m_TestObject.Render(GL_TRIANGLES);
					m_Model.RenderModel(GL_TRIANGLES);
					normalShaderPtr->Unbind();
				}
			}
			// ---------------- Outline Render ------------------
			if (m_DataVisualizer.m_ShowWireframe)
			{
				auto* wireframeShaderPtr = m_DataVisualizer.m_WireframeShader;
				if (wireframeShaderPtr)
				{
					wireframeShaderPtr->Bind();
					wireframeShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
					wireframeShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
					wireframeShaderPtr->SetUniformModel(glm::value_ptr(model));
//					m_TestObject.Render(GL_TRIANGLES);
					m_Model.RenderModel(GL_TRIANGLES);
					wireframeShaderPtr->Unbind();
				}
			}
			// ---------------- Point Render ------------------
			if (m_DataVisualizer.m_ShowPoint)
			{
				auto* pointShaderPtr = m_DataVisualizer.m_PointShader;
				if (pointShaderPtr)
				{
					pointShaderPtr->Bind();
					pointShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
					pointShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
					pointShaderPtr->SetUniformModel(glm::value_ptr(model));
//					m_TestObject.Render(GL_POINTS);
					m_Model.RenderModel(GL_POINTS);
					pointShaderPtr->Unbind();
				}
			}
			if (m_DataVisualizer.m_ShowAABB)
			{
				auto* aabbShaderPtr = m_DataVisualizer.m_AABBShader;
				if (aabbShaderPtr)
				{
					aabbShaderPtr->Bind();
					aabbShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
					aabbShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
					aabbShaderPtr->SetUniformModel(glm::value_ptr(model));
					if (m_AABB)
						m_AABB->DebugRender(m_BBoxDebugDrawLevel);
					aabbShaderPtr->Unbind();
				}
			}
			// ---------------- Bounding Box Render for AABB Debug ------------------
		}
		m_FrameBuffer.Unbind(); // unbind Frame Buffer (render target)
	}

	// NOTE: this is ImGui Render function
    void OnUIRender() override
	{
		const auto currentShaderName = m_DisplayMode.GetCurrentShaderPtr()->GetName();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("ViewPort");
			// https://uysalaltas.github.io/2022/01/09/OpenGL_Imgui.html
			if (ImGui::BeginMenu(currentShaderName.c_str()))
			{
				for (auto &itr : m_DisplayMode.GetShaderMap())
				{
					if (ImGui::MenuItem(itr.first.c_str())) {
						m_DisplayMode.SetCurrentShader(itr.first);
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
			}
			ImGui::End();
			ImGui::PopStyleVar();
		}

		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Data Board");
			{
				// Material // https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/WalnutApp.cpp
				ImGui::BeginGroup();
				{
					ImGui::Checkbox("Mesh", &m_ShowMesh);
					ImGui::SameLine(); ImGui::Text("%s", m_ShowMesh ? "On" : "Off");
					ImGui::Checkbox("Edge", &m_DataVisualizer.m_ShowWireframe);
					ImGui::SameLine(); ImGui::Text("%s", m_DataVisualizer.m_ShowWireframe ? "On" : "Off");
					ImGui::Checkbox("Vetex", &m_DataVisualizer.m_ShowPoint);
					ImGui::SameLine(); ImGui::Text("%s", m_DataVisualizer.m_ShowPoint ? "On" : "Off");
					ImGui::Checkbox("Normal", &m_DataVisualizer.m_ShowNormal);
					ImGui::SameLine(); ImGui::Text("%s", m_DataVisualizer.m_ShowNormal ? "On" : "Off");
					ImGui::Checkbox("AABB", &m_DataVisualizer.m_ShowAABB);
					ImGui::SameLine(); ImGui::Text("%s", m_DataVisualizer.m_ShowAABB ? "On" : "Off");
				}
				ImGui::EndGroup();
				ImGui::BeginGroup();
				{
					if (currentShaderName == "Phong" || currentShaderName == "Cartoon")
					{
						ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_Material.m_AmbientColor));
						ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_Material.m_DiffuseColor));
						ImGui::ColorEdit3("Specular Color", glm::value_ptr(m_Material.m_SpecularColor));
						ImGui::DragFloat("Specular Exponent", &m_Material.m_SpecularExponent);
					}
					else if (currentShaderName == "Explosion")
					{
						auto* ptr = dynamic_cast<ExplosionShader *>(m_DisplayMode.GetCurrentShaderPtr());
						if (ptr != nullptr) {
							ImGui::SliderFloat("Explosion Degree", &(ptr->m_Degree), 0.0f, 10.0f, "%.1f");
						}
					}
					if (m_DataVisualizer.m_ShowWireframe)
					{
						auto* ptr = dynamic_cast<WireframeShader *>(m_DataVisualizer.m_WireframeShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("Wireframe Color", glm::value_ptr(ptr->m_WireframeColor));
						}
					}
					if (m_DataVisualizer.m_ShowPoint)
					{
						auto* ptr = dynamic_cast<PointShader *>(m_DataVisualizer.m_PointShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("Point Color", glm::value_ptr(ptr->m_PointColor));
						}
					}
					if (m_DataVisualizer.m_ShowAABB)
					{
						auto* ptr = dynamic_cast<AABBShader*>(m_DataVisualizer.m_AABBShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("AABB Color", glm::value_ptr(ptr->m_AABBColor));
							ImGui::SliderInt("AABB Depth", &m_BBoxDebugDrawLevel, 0, static_cast<int>(m_AABB->m_MaxDepth));
						}
					}
				}
				ImGui::EndGroup();
			}
			ImGui::End();
			ImGui::PopStyleVar();
		}
	}

	// called once popped from m_LayerStack
	void OnDetach() override
	{
		LOG_TRACE("Layer [{0}] has been detached", _m_Name);
//		for (auto &mesh : m_MeshList)
//			mesh->ClearMesh(); // delete mesh buffer (VAO VBO IBO)
	}

	void OnResize(float width, float height) override
	{
		m_FrameBuffer.RescaleFrameBuffer(width, height);
		m_EditorCamera.OnResize(width, height); // re-calculate camera
	}
};

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec {"Scoop", 1300, 900 };
	auto* app = new Lunar::Application(spec);
    app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()-> void
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{
				// TODO: start empty scene
			}
			if (ImGui::MenuItem("Open"))
			{
				// TODO: open OBJ / RIB / NURBS (renderman) file
			}
			if (ImGui::MenuItem("Export"))
			{
				// TODO: export RIB / NURBS mesh to OBJ
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

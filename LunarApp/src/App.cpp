
#include "imgui_internal.h"
#include "glm/glm.hpp"

#include "Lunar/Camera/EditorCamera.h"
#include "Lunar/Core/Application.h"
#include "Lunar/Core/Timer.h"
#include "Lunar/Core/EntryPoint.h"// main code here
#include "Lunar/FrameBuffer/FrameBuffer.h"
#include "Lunar/Light/Light.h"
#include "Lunar/Material/Material.h"
#include "Lunar/Mesh/Mesh.h"
#include "Lunar/Model/Model.h"
#include "Lunar/Shader/Shader.h"
#include "Lunar/Texture/Texture.h"
#include "Lunar/AABB/AABB.h"
// test for ray tracing + mouse click
#include "Lunar/Input/Input.h"
#include "Lunar/Thread/ThreadPool.h"

#include "LunarApp/src/GUI/GUI.h"
#include "LunarApp/src/RayTracer/RayTracer.h"
#include "LunarApp/src/Shaders/DisplayMode.h"
#include "LunarApp/src/Shaders/Explosion/ExplosionShader.h"
#include "LunarApp/src/Shaders/Phong/PhongShader.h"
#include "LunarApp/src/Shaders/Test/TestShader.h"
#include "LunarApp/src/Shaders/Cartoon/CartoonShader.h"
#include "LunarApp/src/Shaders/DataVisualizer.h"
//#include "LunarApp/src/TestObject/TestObject.h"

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.h
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.h
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.cpp

class ExampleLayer final : public Lunar::Layer
{
private:
	ImVec2 m_GUIScreenSize; // NOTE: ImGUI Viewport Content Size, not screen size.

	DisplayMode m_DisplayMode; // main display mode
	DataVisualizer m_DataVisualizer; // vertex, polygon, normal visualizer
	bool m_RayTracingMode = false;

	Lunar::Light m_MainLight;

	bool m_ShowMesh = true;
	int m_BBoxDebugDrawLevel = 0;

	Lunar::Model m_Model;
	RayTracer m_RayTracer;
	Lunar::FrameBuffer m_RasterizationFrameBuffer; // NOTE: Rasterization Viewport Buffer (Only Rendering)

	std::shared_ptr<Lunar::AABBTree> m_AABB = nullptr;
	std::shared_ptr<Lunar::EditorCamera> m_EditorCamera = nullptr;

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

	// called once pu/shed to m_LayerStack
	void OnAttach() override
	{
		LOG_TRACE("Layer [{0}] has been attached", _m_Name);
		const auto& app = Lunar::Application::Get();
		auto width = app.GetWindowData().BufferWidth;
		auto height = app.GetWindowData().BufferHeight;


	// 1. Create object
//		 m_Model.LoadModel("LunarApp/assets/teapot2.obj");
		m_Model.LoadModel("LunarApp/assets/box.obj");
//		m_Model.LoadModel("LunarApp/assets/bunny.obj");
//				m_Model.LoadModel("LunarApp/assets/dragon.obj");
//		m_Model.LoadModel("LunarApp/assets/sphere.obj"); // 여기서 mtl까지 전부 load.
//		m_Model.LoadModel("LunarApp/assets/42.obj");
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
		m_EditorCamera = std::make_shared<Lunar::EditorCamera>(45.0f, aspectRatio, 0.01f, 100.0f);

		m_DisplayMode.Add( new ExplosionShader() );
		m_DisplayMode.Add( new PhongShader() );
		m_DisplayMode.Add( new TestShader() );
		m_DisplayMode.Add( new CartoonShader() );


#ifdef TEST
		m_AABB = std::make_shared<AABBTree>(m_TestObject.m_Vertices, m_TestObject.m_Indices);
#else
		m_AABB = std::make_shared<Lunar::AABBTree>(m_Model.Vertices, m_Model.Indices);
#endif

		m_RayTracer.Init(m_AABB, m_EditorCamera, width, height);
		m_RasterizationFrameBuffer.Init(width, height); // Init Rasterization buffer
		m_DataVisualizer.Init(); // init wireframe, normal, vertex (Shader)
	}



	// called every render loop
	void OnUpdate(float ts) override
	{
		m_EditorCamera->OnUpdate(ts); // 2. update camera geometry

		// NOTE: mouse click ray-tracing test --> 나중에 리팩토링 할 것.
		// TODO: implement mouse click --> Raytracing
		if (!Lunar::Input::IsKeyPressed(Lunar::Key::LeftAlt)
			&& Lunar::Input::IsMouseButtonPressedOnce(Lunar::Mouse::ButtonLeft))
		{
			// NOTE: mouse pos 는 glfw 윈도우 전체 기준임. 따라서 이 부분 수정 필요함.
			glm::vec2 mouse { Lunar::Input::GetMousePosition() };
			const auto glfwScreenHeight = Lunar::Application::Get().GetWindowData().BufferHeight;
			mouse.y -= ((float)glfwScreenHeight - m_GUIScreenSize.y); // 차이 보완.
			const auto pos = m_EditorCamera->GetPosition();

			// NOTE: Converting screen coordinate to world space Ray
			// -----------------------------------------------------------
			// https://antongerdelan.net/opengl/raycasting.html
			// 1. xy screen coord to NDC
			float NDC_X = ((2.0f * mouse.x) / m_GUIScreenSize.x) - 1.0f;
			float NDC_Y = 1.0f - (2.0f * mouse.y) / m_GUIScreenSize.y;
			glm::vec4 ray_NDC = glm::vec4(NDC_X, NDC_Y, -1.0f, 0.0f); // z(-1) = far

			// 2. NDC ray * Projection inverse * View inverse = World coord ray
			// +) homogeneous coordinate의 마지막 w 가 1.0이면 point이고, 0.0이면 벡터이다.
			glm::vec4 ray_EYE = glm::inverse(m_EditorCamera->GetProjection()) * ray_NDC;
			ray_EYE = glm::vec4(ray_EYE.xy(), -1.0f, 0.0f); // forward direction vector
			glm::vec3 ray_WORLD_DIR = glm::inverse(m_EditorCamera->GetViewMatrix()) * ray_EYE;
			ray_WORLD_DIR = glm::normalize(ray_WORLD_DIR);
			Lunar::Ray ray {m_EditorCamera->GetPosition(), ray_WORLD_DIR };
			auto hit = m_RayTracer.TraceRay(ray);

			const auto shaderProcPtr = dynamic_cast<PhongShader *>(m_DisplayMode.GetByName("Phong"));

			if (hit.distance > 0.0f) {
				shaderProcPtr->SetPickMode(1);
				shaderProcPtr->SetPickedMeshData(hit.triangle.v0.GetVertex(), hit.triangle.v1.GetVertex(), hit.triangle.v2.GetVertex());
				LOG_INFO("Hit Success");
                LOG_INFO("UV x{0} y{1}", hit.uv.x, hit.uv.y);
			}
			else // no hit.
			{
				shaderProcPtr->SetPickMode(0);
			}
		}

		if (m_RayTracingMode)
		{
			m_RayTracer.Render();
		}
		else // Rasterization mode (GPU)
		{
			// 0. bind frame buffer ( = render target image )
			m_RasterizationFrameBuffer.Bind();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 1. Unbind current frame buffer data.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			{
				if (m_ShowMesh)
				{
					glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
					glEnable(GL_CULL_FACE); //  enable backface culling
					m_DisplayMode.BindCurrentShader();
					const auto shaderProcPtr = m_DisplayMode.GetCurrentShaderPtr();
					shaderProcPtr->SetUniformEyePos(m_EditorCamera->GetPosition());
					shaderProcPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera->GetProjection()));
					shaderProcPtr->SetUniformView(glm::value_ptr(m_EditorCamera->GetViewMatrix()));
					shaderProcPtr->SetUniformModel(glm::value_ptr(m_Model.ModelMatrix));
                    m_MainLight.Use(shaderProcPtr);
					m_Model.Render(GL_TRIANGLES, shaderProcPtr);
					m_DisplayMode.UnbindCurrentShader();
				}
				// ----------------  Normal Render ------------------
				if (m_DataVisualizer.m_ShowNormal)
				{
					auto* normalShaderPtr = m_DataVisualizer.m_NormalShader;
					if (normalShaderPtr)
					{
						normalShaderPtr->Bind();
						normalShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera->GetProjection()));
						normalShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera->GetViewMatrix()));
						normalShaderPtr->SetUniformModel(glm::value_ptr(m_Model.ModelMatrix));
						m_Model.Render(GL_TRIANGLES);
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
						wireframeShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera->GetProjection()));
						wireframeShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera->GetViewMatrix()));
						wireframeShaderPtr->SetUniformModel(glm::value_ptr(m_Model.ModelMatrix));
						m_Model.Render(GL_TRIANGLES);
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
						pointShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera->GetProjection()));
						pointShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera->GetViewMatrix()));
						pointShaderPtr->SetUniformModel(glm::value_ptr(m_Model.ModelMatrix));
						m_Model.Render(GL_POINTS);
						pointShaderPtr->Unbind();
					}
				}
				if (m_DataVisualizer.m_ShowAABB)
				{
					auto* aabbShaderPtr = m_DataVisualizer.m_AABBShader;
					if (aabbShaderPtr)
					{
						aabbShaderPtr->Bind();
						aabbShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera->GetProjection()));
						aabbShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera->GetViewMatrix()));
						aabbShaderPtr->SetUniformModel(glm::value_ptr(m_Model.ModelMatrix));
						if (m_AABB)
							m_AABB->DebugRender(m_BBoxDebugDrawLevel);
						aabbShaderPtr->Unbind();
					}
				}
				if (m_DataVisualizer.m_ShowGrid) // Ground Grid
				{
					auto* gridShaderPtr = dynamic_cast<GridShader *>(m_DataVisualizer.m_GridShader);
					if (gridShaderPtr)
					{
						// Draw a full screen covering triangle for bufferless rendering
						// https://trass3r.github.io/coding/2019/09/11/bufferless-rendering.html
						gridShaderPtr->m_FarClip = m_EditorCamera->GetFarClip();
						gridShaderPtr->m_NearClip = m_EditorCamera->GetNearClip();
						gridShaderPtr->Bind();
						gridShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera->GetProjection()));
						gridShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera->GetViewMatrix()));
						gridShaderPtr->DrawDummyVAO();
						// When rendering without any buffers,
						// the vertex shader will simply be invoked the number of specified times without input data
						gridShaderPtr->Unbind();
					}
				}
				// ---------------- Bounding Box Render for AABB Debug ------------------
			}
			m_RasterizationFrameBuffer.Unbind(); // unbind Frame Buffer (render target)
		}
	}


	// NOTE: this is ImGui Render function
    void OnUIRender() override
	{

        /* **************************************
         * Set Initial dock-space layout (Experimental)
         * **************************************/
        // https://gist.github.com/AidanSun05/953f1048ffe5699800d2c92b88c36d9f

        static bool firstLoop = true;
        if (firstLoop)
        {
            // 1. Now we'll need to create our dock node:

            ImGuiID id = ImGui::GetID("DockSpace"); // str_id set in application.cpp
            ImGui::DockBuilderRemoveNode(id);             // Clear any preexisting layouts associated with the ID we just chose
            ImGui::DockBuilderAddNode(id); // add node to dock full screen

            // 2. Split the dock node to create spaces to put our windows in:

            // Split the dock node in the left direction to create our first docking space. This will be on the left side of the node.
            // (The 0.5f means that the new space will take up 50% of its parent - the dock node.)
            ImGuiID dock0 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Up, 0.07f, nullptr, &id);
			ImGuiID dock2 = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.2f, nullptr, &id);
            ImGuiID dock1_a = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.5f, nullptr, &id);
			ImGuiID dock1_b = ImGui::DockBuilderSplitNode(dock1_a, ImGuiDir_Left, 0.5f, nullptr, &dock1_a);
            // +-----------+
			// +-----------+
            // |     1     |
            // |           |
            // +-----------+
            // Split the same dock node in the right direction to create our second docking space.
            // At this point, the dock node has two spaces, one on the left and one on the right.
			// +-----------+
			// +-----+-----+
            // |  1  |  2  |
            // |     |     |
            // +-----+-----+
            //    split ->
            // For our last docking space, we want it to be under the second one but not under the first.
            // Split the second space in the down direction so that we now have an additional space under it.
            //
            // Notice how "dock2" is now passed rather than "id".
            // The new space takes up 50% of the second space rather than 50% of the original dock node.
            // +-----+-----+
            // |     |  2  |  split
            // |  1  +-----+    |
            // |     |  3  |    V
            // +-----+-----+

            // 3. Add windows to each docking space:
            ImGui::DockBuilderDockWindow("Mode Selection", dock0);
            ImGui::DockBuilderDockWindow("Main Viewport", dock1_a);
			ImGui::DockBuilderDockWindow("Second Viewport", dock1_b);
            ImGui::DockBuilderDockWindow("Property", dock2);

            // 4. We're done setting up our docking configuration:
            ImGui::DockBuilderFinish(id);
        }
        if (firstLoop) firstLoop = false;


        /* **************************************
         * Set Switch Tab button for app mode.
         * **************************************/

		static ButtonToggleFlags ToggledBtn = ButtonToggleFlags::LayoutMode; // default button is 0 (Layout)
        ImGui::Begin("Mode Selection");
		{
			GUI_PUSH_STYLE_COLOR_BY_BUTTON_STATE(ToggledBtn, ButtonToggleFlags::LayoutMode);
			ImGui::SameLine(0.0, 2.0f);
			if (ImGui::Button("Layout", ImVec2(100, 20)))
			{
				ToggledBtn = ButtonToggleFlags::LayoutMode;
				m_RayTracingMode = false;
			}
			ImGui::PopStyleColor(1);

			GUI_PUSH_STYLE_COLOR_BY_BUTTON_STATE(ToggledBtn, ButtonToggleFlags::TexturePaintMode);
			ImGui::SameLine(0.0, 2.0f);
			if (ImGui::Button("Texture Paint", ImVec2(130, 20)))
			{
				ToggledBtn = ButtonToggleFlags::TexturePaintMode;
				m_RayTracingMode = false;
			}
			ImGui::PopStyleColor(1);

			GUI_PUSH_STYLE_COLOR_BY_BUTTON_STATE(ToggledBtn, ButtonToggleFlags::RenderMode);
			ImGui::SameLine(0.0, 2.0f);
			if (ImGui::Button("Rendering", ImVec2(100, 20)))
			{
				ToggledBtn = ButtonToggleFlags::RenderMode;
				m_RayTracingMode = true;
			}
			ImGui::PopStyleColor(1);

			GUI_PUSH_STYLE_COLOR_BY_BUTTON_STATE(ToggledBtn, ButtonToggleFlags::ModelingMode);
			ImGui::SameLine(0.0, 2.0f);
			if (ImGui::Button("Modeling", ImVec2(100, 20)))
			{
				ToggledBtn = ButtonToggleFlags::ModelingMode;
				m_RayTracingMode = false;
			}
			ImGui::PopStyleColor(1);
		}
        ImGui::End();

        /* **************************************
         * Set content of each dock.
         * **************************************/

		const auto currentShaderName = m_DisplayMode.GetCurrentShaderPtr()->GetName();
		if (ToggledBtn == ButtonToggleFlags::RenderMode) // **************************************************************************
		{
			m_GUIScreenSize = ImGui::GetContentRegionAvail();
			GUI_PUSH_IMAGE_TO_VIEWPORT("Main Viewport", m_RayTracer.GetFinalImageFrameBuffer()->GetFrameTexture(), m_EditorCamera);
			ImGui::Begin("Property");
			ImGui::BeginGroup();
			{
				ImGui::Text("Last render: %.3fms", m_RayTracer.m_LastRenderTime);
				ImGui::Checkbox("Moller-Trumbore", &m_RayTracer.m_ChangeIntersection);
			}
			ImGui::EndGroup();
			ImGui::End();
		}
		else if (ToggledBtn == ButtonToggleFlags::LayoutMode) // shader, GPU mode. ***************************************************************************
		{
			m_GUIScreenSize = ImGui::GetContentRegionAvail();
			GUI_PUSH_IMAGE_TO_VIEWPORT("Main Viewport", m_RasterizationFrameBuffer.GetFrameTexture(), m_EditorCamera);
			// Material // https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/WalnutApp.cpp
			ImGui::Begin("Property");
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
			ImGui::BeginGroup();
			{
				ImGui::Checkbox("Grid", &m_DataVisualizer.m_ShowGrid);
				ImGui::SameLine(); ImGui::Text("%s", m_DataVisualizer.m_ShowGrid ? "On" : "Off");
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
					ImGui::ColorEdit3("Ambient Color", glm::value_ptr(m_Model.Material.m_AmbientColor));
					ImGui::ColorEdit3("Diffuse Color", glm::value_ptr(m_Model.Material.m_DiffuseColor));
					ImGui::ColorEdit3("Specular Color", glm::value_ptr(m_Model.Material.m_SpecularColor));
					ImGui::DragFloat("Specular Exponent", &m_Model.Material.m_SpecularExponent);
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
				if (m_DataVisualizer.m_ShowGrid)
				{
					auto* ptr = dynamic_cast<GridShader*>(m_DataVisualizer.m_GridShader);
					if (ptr != nullptr) {
						ImGui::ColorEdit3("Grid Color", glm::value_ptr(ptr->m_GridColor));
					}
				}
			}
			ImGui::EndGroup();
			ImGui::End();
		}
		else if (ToggledBtn == ButtonToggleFlags::TexturePaintMode)
		{
			m_GUIScreenSize = ImGui::GetContentRegionAvail();
			GUI_PUSH_IMAGE_TO_VIEWPORT("Main Viewport", m_RasterizationFrameBuffer.GetFrameTexture(), m_EditorCamera);

			ImGui::Begin("Second Viewport");
			// ...
			ImGui::End();

			ImGui::Begin("Property");
			// ...
			ImGui::End();
		}
		else if (ToggledBtn == ButtonToggleFlags::ModelingMode)
		{ /* ... */ }
		else if (ToggledBtn == ButtonToggleFlags::SculptingMode)
		{ /* ... */ }
		else
		{ /* ... */ }
	}


	// called once popped from m_LayerStack
	void OnDetach() override
	{
		LOG_TRACE("Layer [{0}] has been detached", _m_Name);
	}

	void OnResize(float width, float height) override
	{
		m_EditorCamera->OnResize(width, height); // re-calculate camera
		m_RayTracer.OnResize(width, height);
		m_RasterizationFrameBuffer.Resize(width, height);
	}
};

Lunar::Application* Lunar::CreateApplication(int argc, char** argv) noexcept
{
	Lunar::ApplicationSpecification spec {"Scoop", 1800, 1000 };
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

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Render"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::EndMenu();
        }


    });
	return (app);
}

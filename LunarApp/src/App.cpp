
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


struct GuiLayout
{
	float	WindowPadding;
	float	MenubarHeight;
	ImVec2  ModeSelectionPanelSize;
	ImVec2  MainViewportSize;
	ImVec2  SecondViewportSize;
	ImVec2  PropertyPanelSize;
};

class ExampleLayer final : public Lunar::Layer
{

// ImGUI info
private:
	GuiLayout m_GuiLayout;

// Layer members
private:
	DisplayMode m_DisplayMode; // main display mode
	DataVisualizer m_DataVisualizer; // vertex, polygon, normal visualizer
	RayTracer m_RayTracer;
	Lunar::FrameBuffer m_RasterizationFrameBuffer; // NOTE: Rasterization Viewport Buffer (Only Rendering)

	bool m_RayTracingMode = false;
	int m_BBoxDebugDrawLevel = 0;

	Lunar::Model m_Model;
	Lunar::Light m_MainLight;

	std::shared_ptr<Lunar::AABBTree> m_AABB = nullptr;
	std::shared_ptr<Lunar::EditorCamera> m_EditorCamera = nullptr;

	glm::vec4 m_Background_Color { 0.5f, 0.5f, 0.5f, 1.0f };

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
		m_GuiLayout.MenubarHeight = app.GetSpecification().Height; // 초기 값을 window 전체로 수정.
		m_GuiLayout.WindowPadding = app.GetWindowData().Padding;

	// 1. Create object
//		 m_Model.LoadModel("LunarApp/assets/teapot2.obj");
//		m_Model.LoadModel("LunarApp/assets/box.obj");
//		m_Model.LoadModel("LunarApp/assets/bunny.obj");
//				m_Model.LoadModel("LunarApp/assets/dragon.obj");
		m_Model.LoadModel("LunarApp/assets/sphere.obj"); // 여기서 mtl까지 전부 load.
//		m_Model.LoadModel("LunarApp/assets/42.obj");
//		m_Model.LoadModel("LunarApp/assets/shaderBall.obj");

	// 2. Create Texture
//		m_BrickTexture = Lunar::Texture("LunarApp/assets/brick.png");
//		m_BrickTexture.LoadTexture();

	// 2. Create Material
		// ...

	// 3. Create Light
		m_MainLight = Lunar::Light( glm::vec3(2.0f, -1.0f, -1.0f), 0.5f, 0.6f, 0.4f );

	// 4. Init Camera
		auto aspectRatio = (float)width / (float)height;
		m_EditorCamera = std::make_shared<Lunar::EditorCamera>(45.0f, aspectRatio, 0.01f, 100.0f);

		m_DisplayMode.Add( new ExplosionShader() );
		m_DisplayMode.Add( new PhongShader() );
		m_DisplayMode.Add( new TestShader() );
		m_DisplayMode.Add( new CartoonShader() );

		m_AABB = std::make_shared<Lunar::AABBTree>(m_Model.Vertices, m_Model.Indices);

		m_RayTracer.Init(m_AABB, m_EditorCamera, width, height);
		m_RasterizationFrameBuffer.Init(width, height); // Init Rasterization buffer
		m_DataVisualizer.Init(VS_LAYOUT_MODE_OPTION); // init wireframe, normal, vertex (Shader)
	}

	// called every render loop
	void OnUpdate(float ts) override
	{
		m_EditorCamera->OnUpdate(ts); // 2. update camera geometry

		// Mouse Click
		if (!Lunar::Input::IsKeyPressed(Lunar::Key::LeftAlt)
			&& Lunar::Input::IsMouseButtonPressedOnce(Lunar::Mouse::ButtonLeft))
		{
			// Convert screen coordinate to world space Ray
			// https://antongerdelan.net/opengl/raycasting.html
			glm::vec2 mousePos = this->GetRelativeMousePosition(Widget::MainViewport);

			LOG_INFO("ImGUI mouse pos        X{0} Y{1}", mousePos.x, mousePos.y);
			LOG_ERROR("Menubar                Height {0}", m_GuiLayout.MenubarHeight);
			LOG_ERROR("Mode Selection Panel   X{0} Y{1}", m_GuiLayout.ModeSelectionPanelSize.x, m_GuiLayout.ModeSelectionPanelSize.y);
			LOG_ERROR("Main Viewport          X{0} Y{1}", m_GuiLayout.MainViewportSize.x, m_GuiLayout.MainViewportSize.y);
			LOG_ERROR("Second Viewport        X{0} Y{1}", m_GuiLayout.SecondViewportSize.x, m_GuiLayout.SecondViewportSize.y);
			LOG_ERROR("Property Panel         X{0} Y{1}", m_GuiLayout.PropertyPanelSize.x, m_GuiLayout.PropertyPanelSize.y);

			// 1. xy screen coord to NDC
			float NDC_X = ((2.0f * mousePos.x) / (m_GuiLayout.MainViewportSize.x - (2 * m_GuiLayout.WindowPadding))) - 1.0f;
			float NDC_Y = 1.0f - (2.0f * mousePos.y) / (m_GuiLayout.MainViewportSize.y - (2 * m_GuiLayout.WindowPadding));

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
//                LOG_INFO("UV x{0} y{1}", hit.uv.x, hit.uv.y);
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

			glClearColor(m_Background_Color.r, m_Background_Color.g, m_Background_Color.b, m_Background_Color.a); // 1. Unbind current frame buffer data.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
			glEnable(GL_CULL_FACE); //  enable backface culling
			{
				if (m_DataVisualizer.IsSet(Option::Mesh))
				{
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
				if (m_DataVisualizer.IsSet(Option::Normal))
				{
					auto* normalShaderPtr = m_DataVisualizer.NormalShader;
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
				if (m_DataVisualizer.IsSet(Option::Wireframe))
				{
					auto* wireframeShaderPtr = m_DataVisualizer.WireframeShader;
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
				if (m_DataVisualizer.IsSet(Option::Point))
				{
					auto* pointShaderPtr = m_DataVisualizer.PointShader;
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
				if (m_DataVisualizer.IsSet(Option::AABB))
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
				if (m_DataVisualizer.IsSet(Option::Grid)) // Ground Grid
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

	// ImGui Render function
    void OnUIRender() override
	{
        /* **************************************
         * Set Initial dock-space layout (Experimental)
         * **************************************/
        // https://gist.github.com/AidanSun05/953f1048ffe5699800d2c92b88c36d9f
		static bool firstLoop = true;

        if (firstLoop)
		{
			GUI_SET_INITIAL_UI();
		}

        /* **************************************
         * Set Switch Tab button for app mode.
         * **************************************/

		static ButtonToggleFlags ToggledBtnState = ButtonToggleFlags::LayoutMode; // default button is 0 (Layout)
        ImGui::Begin("Mode Selection");
		{
			m_GuiLayout.ModeSelectionPanelSize = ImGui::GetWindowSize();
			GUI_MODE_BUTTON_COMPONENT(ButtonToggleFlags::LayoutMode, &ToggledBtnState, [&]() -> void { m_RayTracingMode = false; });
			GUI_MODE_BUTTON_COMPONENT(ButtonToggleFlags::TexturePaintMode, &ToggledBtnState, [&]() -> void { m_RayTracingMode = false; });
			GUI_MODE_BUTTON_COMPONENT(ButtonToggleFlags::RenderMode, &ToggledBtnState, [&]()-> void { m_RayTracingMode = true; });
			GUI_MODE_BUTTON_COMPONENT(ButtonToggleFlags::ModelingMode, &ToggledBtnState, [&]() -> void { m_RayTracingMode = false; });
		}
        ImGui::End();

        /* **************************************
         * Set content of each dock.
         * **************************************/

		const auto currentShaderName = m_DisplayMode.GetCurrentShaderPtr()->GetName();
		if (ToggledBtnState == ButtonToggleFlags::RenderMode) // **************************************************************************
		{
			m_GuiLayout.SecondViewportSize = ImVec2 { 0.0f, 0.0f };
			m_GuiLayout.MainViewportSize = GUI_VIEWPORT_IMAGE_COMPONENT("Main Viewport", m_RayTracer.GetFinalImageFrameBuffer()->GetFrameTexture(), m_EditorCamera);

			ImGui::Begin("Property");
			m_GuiLayout.PropertyPanelSize = ImGui::GetWindowSize();
			ImGui::BeginGroup();
			{
				ImGui::Text("Last render: %.3fms", m_RayTracer.m_LastRenderTime);
				ImGui::Checkbox("Moller-Trumbore", &m_RayTracer.m_ChangeIntersection);
			}
			ImGui::EndGroup();
			ImGui::End();
		}
		else if (ToggledBtnState == ButtonToggleFlags::LayoutMode) // shader, GPU mode. ***************************************************************************
		{
			// set to default layout mode gui
			m_DataVisualizer.OverrideMode(VS_LAYOUT_MODE_OPTION);
			m_GuiLayout.SecondViewportSize = ImVec2 { 0.0f, 0.0f };
			m_GuiLayout.MainViewportSize = GUI_VIEWPORT_IMAGE_COMPONENT("Main Viewport", m_RasterizationFrameBuffer.GetFrameTexture(), m_EditorCamera);

			ImGui::Begin("Property");
			{
				m_GuiLayout.PropertyPanelSize = ImGui::GetWindowSize();

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
					GUI_VISUALIZER_BUTTON_COMPONENT(Option::Mesh, &m_DataVisualizer);
					GUI_VISUALIZER_BUTTON_COMPONENT(Option::Grid, &m_DataVisualizer);
					GUI_VISUALIZER_BUTTON_COMPONENT(Option::Wireframe, &m_DataVisualizer);
					GUI_VISUALIZER_BUTTON_COMPONENT(Option::Point, &m_DataVisualizer);
					GUI_VISUALIZER_BUTTON_COMPONENT(Option::Normal, &m_DataVisualizer);
					GUI_VISUALIZER_BUTTON_COMPONENT(Option::AABB, &m_DataVisualizer);
				}
				ImGui::EndGroup();

				ImGui::BeginGroup();
				{
					ImGui::ColorEdit3("Background Color", glm::value_ptr(m_Background_Color));
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
					if (m_DataVisualizer.IsSet(Option::Wireframe))
					{
						auto* ptr = dynamic_cast<WireframeShader *>(m_DataVisualizer.WireframeShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("Wireframe Color", glm::value_ptr(ptr->m_WireframeColor));
						}
					}
					if (m_DataVisualizer.IsSet(Option::Point))
					{
						auto* ptr = dynamic_cast<PointShader *>(m_DataVisualizer.PointShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("Point Color", glm::value_ptr(ptr->m_PointColor));
						}
					}
					if (m_DataVisualizer.IsSet(Option::AABB))
					{
						auto* ptr = dynamic_cast<AABBShader*>(m_DataVisualizer.m_AABBShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("AABB Color", glm::value_ptr(ptr->m_AABBColor));
							ImGui::SliderInt("AABB Depth", &m_BBoxDebugDrawLevel, 0, static_cast<int>(m_AABB->m_MaxDepth));
						}
					}
					if (m_DataVisualizer.IsSet(Option::Grid))
					{
						auto* ptr = dynamic_cast<GridShader*>(m_DataVisualizer.m_GridShader);
						if (ptr != nullptr) {
							ImGui::ColorEdit3("Grid Color", glm::value_ptr(ptr->m_GridColor));
						}
					}
				}
				ImGui::EndGroup();
			}
			ImGui::End(); // PROPERTY
		}
		else if (ToggledBtnState == ButtonToggleFlags::TexturePaintMode)
		{
			// set pain_mode gui option
			m_DataVisualizer.OverrideMode(VS_TEXTURE_PAINT_MODE_OPTION);
			m_GuiLayout.MainViewportSize = GUI_VIEWPORT_IMAGE_COMPONENT("Main Viewport", m_RasterizationFrameBuffer.GetFrameTexture(), m_EditorCamera);

			ImGui::Begin("Second Viewport");
			{
				m_GuiLayout.SecondViewportSize = ImGui::GetWindowSize();
				// ...
			}
			ImGui::End();

			ImGui::Begin("Property");
			{
				m_GuiLayout.PropertyPanelSize = ImGui::GetWindowSize();
			}
			ImGui::End();
		}
		else if (ToggledBtnState == ButtonToggleFlags::ModelingMode)
		{ /* ... */ }
		else if (ToggledBtnState == ButtonToggleFlags::SculptingMode)
		{ /* ... */ }
		else
		{ /* ... */ }

        // IMGUI 첫 프레임 렌더시 IMGUI::GetWindowSize의 반환값이 잘못 나옴. 따라서 마우스 클릭이 되지 않음. 이 부분 예외 처리.
        static bool imgui_test = true;
		if (imgui_test && m_GuiLayout.MainViewportSize.x > 0 && m_GuiLayout.MainViewportSize.y > 0)
		{
			m_GuiLayout.MenubarHeight -= ( m_GuiLayout.MainViewportSize.y + m_GuiLayout.ModeSelectionPanelSize.y );
            imgui_test = false;
		}

        if (firstLoop) {
            firstLoop = false;
        }
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

private:
	enum class Widget
	{
		MainViewport        = 0, // main 화면.
		SecondViewport      = 1, // 보조 화면.
		PropertyPanel       = 1 << 1, // 우측 컨트롤 패널.
		ModeSelectionPanel  = 1 << 2, // 상단 모드 선택 버튼 패널
	};

	// ImGUI 위젯의 내부 상대 위치를 조정해서 반환. // name: 위젯의 이름.
	// Main Viewport, Second Viewport, Property, Mode Selection 총 4개중 선택.
	glm::vec2 GetRelativeMousePosition(Widget target) const
	{
		const ImVec2 mousePosImGUI = ImGui::GetMousePos();
		glm::vec2 mousePos { mousePosImGUI.x, mousePosImGUI.y };
		switch (target)
		{
			case Widget::MainViewport:
				mousePos.x -= m_GuiLayout.SecondViewportSize.x;
				mousePos.y -= m_GuiLayout.MenubarHeight;
				mousePos.y -= m_GuiLayout.ModeSelectionPanelSize.y;
				break;
			case Widget::SecondViewport:
				mousePos.y -= m_GuiLayout.MenubarHeight;
				mousePos.y -= m_GuiLayout.ModeSelectionPanelSize.y;
				break;
			case Widget::PropertyPanel:
				mousePos.x -= m_GuiLayout.MainViewportSize.x;
				mousePos.x -= m_GuiLayout.SecondViewportSize.x;
				mousePos.y -= m_GuiLayout.MenubarHeight;
				break;
			case Widget::ModeSelectionPanel:
				mousePos.y -= m_GuiLayout.MenubarHeight;
				break;
			default:
				assert(false && "Target widget doesn't exist");
		}
		mousePos -= m_GuiLayout.WindowPadding;
		return mousePos;
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


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

#include "LunarApp/src/shaders/DisplayMode.h"
#include "LunarApp/src/shaders/Explosion/ExplosionShader.h"
#include "LunarApp/src/shaders/Phong/PhongShader.h"
#include "LunarApp/src/shaders/Test/TestShader.h"
#include "LunarApp/src/shaders/Cartoon/CartoonShader.h"
#include "LunarApp/src/shaders/DataVisualizer.h"
#include "LunarApp/src/AABB/AABB.h"
#include "LunarApp/src/TestObject/TestObject.h"


// test for ray tracing + mouse click
#include "Lunar/Input/Input.h"

#include "Lunar/Input/KeyCodes.h" // including Mouse Code.
#include "Lunar/Input/MouseCodes.h"
#include <execution>

// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.h
// Image 배열에 render --> 이후 framebuffer로 복사 --> framebuffer를 imGUI로 업데이트

// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.h
// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.cpp

//#define TEST

namespace Utils {
	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
		uint8_t g = (uint8_t)(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
		uint8_t b = (uint8_t)(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
		uint8_t a = (uint8_t)(glm::clamp(color.a, 0.0f, 1.0f) * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
}

// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.cpp
class RayTracer
{
public:
	float m_LastRenderTime = 0.0f;

private:
	// TODO: remove light and material later. this is just for phong test.
	Lunar::Light m_MainLight = { glm::vec3(2.0f, -1.0f, -1.0f), 0.1f, 0.7f, 0.5f };
	Lunar::Material m_Material;
	// TODO ---------------------------------------------------------------

	uint32_t* m_ImageData = nullptr;                            // ray-tracing render buffer
	std::shared_ptr<Lunar::FrameBuffer> m_FinalImageFrameBuffer;// framebuffer
	std::shared_ptr<AABBTree> m_ActiveAABBScene;
	const Lunar::EditorCamera* m_ActiveEditorCamera = nullptr;

	// for std::for_each
	std::vector<uint32_t> m_ImageColumnIterator, m_ImageRowIterator;

public:
	RayTracer() = default;

	RayTracer(const std::shared_ptr<AABBTree>& aabbScene, const Lunar::EditorCamera& camera)
		: m_ActiveAABBScene(aabbScene), m_ActiveEditorCamera(&camera)
	{}

	RayTracer& operator=(const RayTracer& other) = delete;

	void LoadAABBScene(const std::shared_ptr<AABBTree>& aabbScene)
	{
		m_ActiveAABBScene = aabbScene;
	}

	void OnResize(uint32_t width, uint32_t height)
	{
		if (m_FinalImageFrameBuffer)
		{
			// No resize necessary
			if (m_FinalImageFrameBuffer->GetWidth() == width && m_FinalImageFrameBuffer->GetHeight() == height)
				return;
			m_FinalImageFrameBuffer->Resize(width, height);
		}
		else
		{
			m_FinalImageFrameBuffer = std::make_shared<Lunar::FrameBuffer>(width, height);
		}
		delete[] m_ImageData;
		m_ImageData = new uint32_t[width * height];

		m_ImageRowIterator.resize(width);
		m_ImageColumnIterator.resize(height);
		for (uint32_t i = 0; i < width; i++)
			m_ImageRowIterator[i] = i;
		for (uint32_t i = 0; i < height; i++)
			m_ImageColumnIterator[i] = i;
	}

	void Render(const Lunar::EditorCamera& camera)
	{
		// TODO: 이 부분에서 main thread 하나가 계속 화면에 pixel를 덮어쓰는 역할을 하고
		// 		  worker들이 계속 배열에다가 그림을 그리는데, 화면이 바뀌어야 할 때 마다
		// 		  기존에 하던 일을 버리고 처음부터 일을 다시 배정받는 방식으로 설계 필요.

		Lunar::Timer timer;
		m_ActiveEditorCamera = &camera;

#if (MT == 1) // NOTE: this variable is set by CMakelist.txt
		std::for_each(std::execution::par_unseq, m_ImageColumnIterator.begin(), m_ImageColumnIterator.end(),
					  [this](uint32_t y)// capture reference of this
					  {
						std::for_each(std::execution::par_unseq, m_ImageRowIterator.begin(), m_ImageRowIterator.end(),
									  [this, y](uint32_t x)
									  {
											glm::vec4 color = CalculateColorPerPixel(x, y);
											m_ImageData[(x) + ((y) * m_FinalImageFrameBuffer->GetWidth())] = Utils::ConvertToRGBA(color);
									  });
					  });
#else
		for (uint32_t y = 0; y < m_FinalImageFrameBuffer->GetHeight(); y+=4)
		{
			for (uint32_t x = 0; x < m_FinalImageFrameBuffer->GetWidth(); x+=4)
			{
				// For data locality, draw the pixels in a tile of e.g. 4×4 pixels often find the same triangles
				for (uint32_t v = 0; v < 4; v++)
				{
					for (uint32_t u = 0; u < 4; u++)
					{
						glm::vec4 color = CalculateColorPerPixel(x + u, y + v);
						m_ImageData[(x + u) + ((y + v) * m_FinalImageFrameBuffer->GetWidth())] = Utils::ConvertToRGBA(color);
					}
				}
			}
		}
#endif
		m_FinalImageFrameBuffer->LoadPixelsToTexture(m_ImageData);
		m_LastRenderTime = timer.ElapsedMillis();
	}

	std::shared_ptr<Lunar::FrameBuffer> GetFinalImageFrameBuffer() const
	{ return m_FinalImageFrameBuffer; }


private:
	glm::vec4 GetPhongShadedColor(Hit hit)
	{
		glm::vec4 ambientColor = glm::vec4(m_Material.m_AmbientColor, 1.0f) * m_MainLight.GetAmbientIntensity();
		glm::vec3 lightDir = glm::normalize(m_MainLight.GetDirection());
		float diffuseFactor = glm::max(glm::dot(-hit.blendedPointNormal, lightDir), 0.0f);
		glm::vec4 diffuseColor = glm::vec4(m_Material.m_DiffuseColor, 1.0f) * m_MainLight.GetDiffuseIntensity() * diffuseFactor;
		glm::vec4 specularColor { 0.0f };
		if (diffuseFactor > 0)
		{
			glm::vec3 lightReflectionDir = glm::normalize(2 * dot(-hit.blendedPointNormal, lightDir) * hit.blendedPointNormal - lightDir);
			glm::vec3 eyeToPointDir = glm::normalize(hit.point - m_ActiveEditorCamera->GetPosition());
			float specularFactor = glm::max(glm::dot(lightReflectionDir, -eyeToPointDir), 0.0f);
			float specularPowFactor = glm::pow(specularFactor, m_Material.m_SpecularExponent);
			specularColor = glm::vec4(m_Material.m_SpecularColor, 1.0f) * m_MainLight.GetSpecularIntensity() * specularPowFactor;
		}
		return ambientColor + diffuseColor + specularColor;
	}

	glm::vec4 CalculateColorPerPixel(uint32_t x, uint32_t y)
	{
		Ray ray = ConvertPixelPositionToWorldSpaceRay(x, y);
		Hit hit = TraceRay(ray);
		if (hit.distance < 0.0f)
			return glm::vec4(0.0f); // BLACK
		else
			return GetPhongShadedColor(hit);
	}

public:
	// NOTE: Converting screen coordinate to world space Ray
	Ray ConvertPixelPositionToWorldSpaceRay(uint32_t pixelX, uint32_t pixelY)
	{
		// https://antongerdelan.net/opengl/raycasting.html
		float NDC_X = ((2.0f * pixelX) / m_FinalImageFrameBuffer->GetWidth()) - 1.0f;
		float NDC_Y = 1.0f - (2.0f * pixelY) / m_FinalImageFrameBuffer->GetHeight();
		// ---------------------------------------------------------
		NDC_Y = -NDC_Y; // NOTE: 이 부분 해결 필요. 왜 뒤집히는 건지?
		// ---------------------------------------------------------
		glm::vec4 ray_NDC = glm::vec4(NDC_X, NDC_Y, -1.0f, 1.0f); // z(-1) = far
		// 2. NDC ray * Projection inverse * View inverse = World coord ray
		// +) homogeneous coordinate의 마지막 w 가 1.0이면 point이고, 0.0이면 벡터이다.
		glm::vec4 ray_EYE = glm::inverse(m_ActiveEditorCamera->GetProjection()) * ray_NDC;
		ray_EYE = glm::vec4(ray_EYE.xy(), -1.0f, 0.0f); // forward direction vector
		glm::vec3 ray_WORLD_DIR = glm::inverse(m_ActiveEditorCamera->GetViewMatrix()) * ray_EYE;
		ray_WORLD_DIR = glm::normalize(ray_WORLD_DIR);
		return Ray {m_ActiveEditorCamera->GetPosition(), ray_WORLD_DIR };
	}

	Hit TraceRay(const Ray& ray)
	{
		Hit hitResult = m_ActiveAABBScene->IntersectBVH(ray);
		return hitResult;
	}
};


class ExampleLayer final : public Lunar::Layer
{
private:
	ImVec2 m_ViewportSize; // NOTE: ImGUI Viewport Content Size, not screen size.
    Lunar::FrameBuffer m_RasterizationFrameBuffer; // NOTE: Rasterization Viewport Buffer (Only Rendering)

	DisplayMode m_DisplayMode; // main display mode
	DataVisualizer m_DataVisualizer; // vertex, polygon, normal visualizer

	RayTracer m_RayTracer;
	bool m_RayTracingMode = false;

	Lunar::EditorCamera m_EditorCamera;
	Lunar::Light m_MainLight;
	Lunar::Material m_Material;
	// Lunar::Texture m_BrickTexture;

	bool m_ShowMesh = true;
	int m_BBoxDebugDrawLevel = 0;

	Lunar::Model m_Model;
	TestObject m_TestObject; // NOTE: Temporary data For AABB Test
	std::shared_ptr<AABBTree> m_AABB = nullptr;
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

	// called once pu/shed to m_LayerStack
	void OnAttach() override
	{
		LOG_TRACE("Layer [{0}] has been attached", _m_Name);
		const auto& app = Lunar::Application::Get();
		auto width = app.GetWindowData().BufferWidth;
		auto height = app.GetWindowData().BufferHeight;

		m_RayTracer.OnResize(width, height); // init RayTracer.
		m_RasterizationFrameBuffer.Init(width, height); // Init Rasterization buffer

	// 1. Create object
//		 m_Model.LoadModel("LunarApp/assets/teapot2.obj");
//		m_Model.LoadModel("LunarApp/assets/bunny/bunny.obj");
//				m_Model.LoadModel("LunarApp/assets/dragon.obj");
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
		m_EditorCamera = Lunar::EditorCamera(45.0f, aspectRatio, 0.01f, 100.0f);
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
#ifdef TEST
		m_AABB = std::make_shared<AABBTree>(m_TestObject.m_Vertices, m_TestObject.m_Indices);
#else
		m_AABB = std::make_shared<AABBTree>(m_Model.vertices, m_Model.indices);
#endif
		m_RayTracer.LoadAABBScene(m_AABB);
		// ------------------------------------
	}

	// x, y 픽셀이 입력되면, 이 픽셀을 이용해 World Space에 대한 Ray를 계산하여 반환.


	// called every render loop
	void OnUpdate(float ts) override
	{
		m_EditorCamera.OnUpdate(ts); // 2. update camera geometry

		// NOTE: mouse click ray-tracing test --> 나중에 리팩토링 할 것.
		// TODO: implement mouse click --> Raytracing
		if (!Lunar::Input::IsKeyPressed(Lunar::Key::LeftAlt)
			&& Lunar::Input::IsMouseButtonPressedOnce(Lunar::Mouse::ButtonLeft))
		{
			// NOTE: mouse pos 는 glfw 윈도우 전체 기준임. 따라서 이 부분 수정 필요함.
			glm::vec2 mouse { Lunar::Input::GetMousePosition() };
			const auto glfwScreenHeight = Lunar::Application::Get().GetWindowData().BufferHeight;
			mouse.y -= ((float)glfwScreenHeight - m_ViewportSize.y); // 차이 보완.
			const auto pos = m_EditorCamera.GetPosition();

			// calcalate ray intersection

//			// NOTE: Converting screen coordinate to world space Ray
//			// -----------------------------------------------------------
//			// https://antongerdelan.net/opengl/raycasting.html
			// 1. xy screen coord to NDC
			float NDC_X = ((2.0f * mouse.x) / m_ViewportSize.x) - 1.0f;
			float NDC_Y = 1.0f - (2.0f * mouse.y) / m_ViewportSize.y;
			glm::vec4 ray_NDC = glm::vec4(NDC_X, NDC_Y, -1.0f, 0.0f); // z(-1) = far

			// 2. NDC ray * Projection inverse * View inverse = World coord ray
			// +) homogeneous coordinate의 마지막 w 가 1.0이면 point이고, 0.0이면 벡터이다.
			glm::vec4 ray_EYE = glm::inverse(m_EditorCamera.GetProjection()) * ray_NDC;
			ray_EYE = glm::vec4(ray_EYE.xy(), -1.0f, 0.0f); // forward direction vector
			glm::vec3 ray_WORLD_DIR = glm::inverse(m_EditorCamera.GetViewMatrix()) * ray_EYE;
			ray_WORLD_DIR = glm::normalize(ray_WORLD_DIR);
			Ray ray {m_EditorCamera.GetPosition(), ray_WORLD_DIR };
			auto hit = m_RayTracer.TraceRay(ray);
			const auto shaderProcPtr = dynamic_cast<PhongShader *>(m_DisplayMode.GetByName("Phong"));
			if (hit.distance > 0.0f) {
				shaderProcPtr->SetPickMode(1);
				shaderProcPtr->SetPickedMeshData(hit.triangle.v0.GetVertex(), hit.triangle.v1.GetVertex(), hit.triangle.v2.GetVertex());
				LOG_INFO("HIT SUCCESS");
				auto p = m_EditorCamera.GetPosition();
				auto k = m_EditorCamera.GetPitch(); auto e = m_EditorCamera.GetYaw();
				LOG_INFO("camera pos    X{0} Y{1} Z{2}", p.x, p.y, p.z);
				LOG_INFO("camera angle  Pitch{0} Yaw{1}", k, e);
				/*
				LOG_INFO("*********************************************************");
				LOG_INFO("*             HIT SUCCESS!!                             *");
				LOG_INFO("*********************************************************");
				LOG_INFO("Distance 		   {0}", hit.distance);
				LOG_INFO("Point 		  X{0} Y{1} Z{2}", hit.point.x, hit.point.y, hit.point.z);
				LOG_INFO("Surface normal  X{0} Y{1} Z{2}", hit.faceNormal.x, hit.faceNormal.y, hit.faceNormal.z);
				LOG_INFO("Triangle v0     X{0} Y{1} Z{2}", hit.triangle.v0.x, hit.triangle.v0.y, hit.triangle.v0.z);
				LOG_INFO("Triangle v1     X{0} Y{1} Z{2}", hit.triangle.v1.x, hit.triangle.v1.y, hit.triangle.v1.z);
				LOG_INFO("Triangle v2     X{0} Y{1} Z{2}\n", hit.triangle.v2.x, hit.triangle.v2.y, hit.triangle.v2.z);
				LOG_INFO("Surface normal  X{0} Y{1} Z{2}", hit.faceNormal.x, hit.faceNormal.y, hit.faceNormal.z);
				LOG_INFO("Blended normal  X{0} Y{1} Z{2}", hit.blendedPointNormal.x, hit.blendedPointNormal.y, hit.blendedPointNormal.z);
				*/
			}
			else // no hit.
			{
				shaderProcPtr->SetPickMode(0);
			}
		}

		if (m_RayTracingMode)
		{
			m_RayTracer.Render(m_EditorCamera);
		}
		else // Rasterization mode (GPU)
		{
			// 0. bind frame buffer ( = render target image )
			m_RasterizationFrameBuffer.Bind();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 1. Unbind current frame buffer data.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			{
				glm::mat4 model(1.0f); // init unit matrix
//				model = glm::scale(model, glm::vec3(3.0f));
				// TODO: model이 변화했을때, mouse picking시 이 부분이 반영되어야 한다... 레이트레이싱도 마찬가지로 해야 되지 않나..?)

				// ---------------- Main Object Render ------------------
				// model이 변화했을때, 충돌 감지에 따라 이 부분이 반영되어야 한다.

				if (m_ShowMesh)
				{
					glEnable(GL_DEPTH_TEST); // 약간 야매 방식. depth buffer 없이 실시간 검사로 일단 테스트 (임시 방편)
					glEnable(GL_CULL_FACE); //  enable backface culling
					m_DisplayMode.BindCurrentShader();
					const auto shaderProcPtr = m_DisplayMode.GetCurrentShaderPtr();
					shaderProcPtr->SetUniformEyePos(m_EditorCamera.GetPosition());
					shaderProcPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
					shaderProcPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
					shaderProcPtr->SetUniformModel(glm::value_ptr(model));
					m_Material.UseMaterial(*shaderProcPtr);
					//			m_BrickTexture.UseTexture();
					m_MainLight.UseLight(*shaderProcPtr);

#ifdef TEST
					m_TestObject.Render(GL_TRIANGLES);
#else
					m_Model.RenderModel(GL_TRIANGLES);
#endif
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
#ifdef TEST
						m_TestObject.Render(GL_TRIANGLES);
#else
						m_Model.RenderModel(GL_TRIANGLES);
#endif
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
#ifdef TEST
						m_TestObject.Render(GL_TRIANGLES);
#else
						m_Model.RenderModel(GL_TRIANGLES);
#endif
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
#ifdef TEST
						m_TestObject.Render(GL_POINTS);
#else
						m_Model.RenderModel(GL_POINTS);
#endif
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
				if (m_DataVisualizer.m_ShowGrid) // Ground Grid
				{
					auto* gridShaderPtr = dynamic_cast<GridShader *>(m_DataVisualizer.m_GridShader);
					if (gridShaderPtr)
					{
						// NOTE: Grid는 VAO/VBO가 필요 없다...
						// NOTE: Draw a full screen covering triangle for bufferless rendering...
						// https://trass3r.github.io/coding/2019/09/11/bufferless-rendering.html
						// https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/
						gridShaderPtr->m_FarClip = m_EditorCamera.GetFarClip();
						gridShaderPtr->m_NearClip = m_EditorCamera.GetNearClip();
						gridShaderPtr->Bind();
						gridShaderPtr->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
						gridShaderPtr->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
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
		const auto currentShaderName = m_DisplayMode.GetCurrentShaderPtr()->GetName();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Display Mode");

			ImGui::Checkbox("Ray-Tracing", &m_RayTracingMode);
			if (m_RayTracingMode) // **************************************************************************
			{
				ImGui::Text("Last render: %.3fms", m_RayTracer.m_LastRenderTime);
				ImGui::Begin("Viewport");
				m_ViewportSize = ImGui::GetContentRegionAvail();
				ImGui::Image(
						reinterpret_cast<void *>(m_RayTracer.GetFinalImageFrameBuffer()->GetFrameTexture()),
						m_ViewportSize,
						ImVec2(0, 1),
						ImVec2(1, 0)
				);
				// NOTE: 윈도우 사이즈는 동일하지만 ImGUI Viewport 사이즈가 바뀌었을 경우
				m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
				m_RayTracer.OnResize(m_ViewportSize.x, m_ViewportSize.y);
				ImGui::End();
			}
			else // shader, GPU mode. ***************************************************************************
			{
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
					ImGui::Begin("Data Board");
					{
						// Material // https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/WalnutApp.cpp
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
							if (m_DataVisualizer.m_ShowGrid)
							{
								auto* ptr = dynamic_cast<GridShader*>(m_DataVisualizer.m_GridShader);
								if (ptr != nullptr) {
									ImGui::ColorEdit3("Grid Color", glm::value_ptr(ptr->m_GridColor));
								}
							}
						}
						ImGui::EndGroup();
					}
					ImGui::End();
				}
				{
					ImGui::Begin("Viewport");
					m_ViewportSize = ImGui::GetContentRegionAvail();
					// NOTE: put m_FrameBuffer's image data to ImGui Image
					// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
					ImGui::Image(
							reinterpret_cast<void *>(m_RasterizationFrameBuffer.GetFrameTexture()),
							m_ViewportSize,
							ImVec2(0, 1),
							ImVec2(1, 0)
					);
					// NOTE: 윈도우 사이즈는 동일하지만 ImGUI Viewport 사이즈가 바뀌었을 경우
					m_EditorCamera.OnResize(m_ViewportSize.x, m_ViewportSize.y);
//					m_RasterizationFrameBuffer.Resize(m_ViewportSize.x, m_ViewportSize.y);
					ImGui::End();
				}
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
		m_EditorCamera.OnResize(width, height); // re-calculate camera
		m_RayTracer.OnResize(width, height);
		m_RasterizationFrameBuffer.Resize(width, height);
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

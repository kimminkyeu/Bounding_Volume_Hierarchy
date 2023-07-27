
#include <glm/glm.hpp>

#include "Lunar/Core/Application.h"
#include "Lunar/Core/EntryPoint.h" // main code here
#include "Lunar/Shader/ShaderProgram.h"
#include "Lunar/Mesh/Mesh.h"
#include "Lunar/Camera/EditorCamera.h"
#include "Lunar/Texture/Texture.h"
#include "Lunar/Model/Model.h"
#include "Lunar/Light/Light.h"

// For Phong Model..
// 중요한건, 이미 GPU에서 raster화 할때 각 지점간의 normal을 interpolating한다. (이건 내장 행위임)
// 아래 함수는 우리가 테스트하는 도형이 육각형이 아니라, 피라미드 형태라서 normal를 직접 입력하기 애매해서 구현하는 거다.

// TODO: 이후 obj loader 프로그램을 작성할때, 아래 함수를 참고해서
// normal 데이터도 함께 동적으로 계산, 추가하자.
// 이때 구조는 assimp를 참고할 것.
void calculateAverageNormals(unsigned int* indices, unsigned int indicesCount, GLfloat * vertices, unsigned int verticesCount,
							 	unsigned int vLength, unsigned int normalOffset)
{
	// 주어진 좌표와 face를 이용해서 normal을 계산하는 과정.
	for (size_t i = 0; i < indicesCount; i += 3)// x y z (3) --> jump to next triangle
	{
		// 3번째 점의 시작 지점 = 3 * 8 = 24. indices 배열의 특정 값에 해당하는 vertices 좌표 알아내는 방법.
		unsigned int index0 = indices[i] * vLength;
		unsigned int index1 = indices[i + 1] * vLength;
		unsigned int index2 = indices[i + 2] * vLength;
		// index0 을 기준으로 V[0->1] 와  V[0->2] 두개를 CrossProduct하면, 일단 Normal을 구할 수 있다.
		glm::vec3 v1(vertices[index1] - vertices[index0], vertices[index1 + 1] - vertices[index0 + 1], vertices[index1 + 2] - vertices[index0 + 2]);
		glm::vec3 v2(vertices[index2] - vertices[index0], vertices[index2 + 1] - vertices[index2 + 1], vertices[index2 + 2] - vertices[index2 + 2]);
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		// 이제 normal 값 반영.
		index0 += normalOffset;
		index1 += normalOffset;
		index2 += normalOffset;
		vertices[index0] += normal.x;
		vertices[index0 + 1] += normal.y;
		vertices[index0 + 2] += normal.z;
		vertices[index1] += normal.x;
		vertices[index1 + 1] += normal.y;
		vertices[index1 + 2] += normal.z;
		vertices[index2] += normal.x;
		vertices[index2 + 1] += normal.y;
		vertices[index2 + 2] += normal.z;
	}
	// 이제 더한것 평균 내기 (interpolating to get smoother edges)
	for (size_t i=0; i<verticesCount / vLength; ++i)
	{
		unsigned int nOffset = (i * vLength) + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

class ExampleLayer final : public Lunar::Layer
{
private:

	std::unique_ptr<Lunar::ShaderProgram> m_ShaderProgram;
	std::vector<std::unique_ptr<Lunar::Mesh>> m_MeshList;
	Lunar::EditorCamera m_EditorCamera;
//	Lunar::Model m_Model;
	Lunar::Texture m_BrickTexture;
	Lunar::Light m_MainLight;

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

		// load shader // 아 복사 생성자 만들고 소멸자 호출되서 쉐이더가 터진거였네;;;
		unsigned int indices[] = {
				0, 3, 1,
				1, 3, 2,
				2, 3, 0,
				0, 1, 2,
		};
		GLfloat verticies[] = {
		//          X          Y          Z           |    U        V        |   Normals (그러나 우리가 직접 계산할 예정)
				-1.0f, -1.0f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
				0.0f, -1.0f, 1.0f,	0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
				1.0f, -1.0f, 0.0f,	1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,	0.5f, 1.0f,		0.0f, 0.0f, 0.0f,
		};
		// TODO: 왜 x축 방향 빛이 계산이 안되지...? Normal 문제가 아닌가?
		calculateAverageNormals(indices, 12, verticies, 32, 8, 5);

		for (int i=0; i<4; ++i)
		{
			for (int j=0; j<8; ++j)
			{
				std::cout << verticies[i * 8 + j] << " ";
			}
			std::cout << "\n";
		}
	// 1. Create object
		m_MeshList.push_back(std::make_unique<Lunar::Mesh>(verticies, indices, 32, 12));
//		m_Model.LoadModel("LunarApp/assets/teapot2.obj");

	// 2. Create Texture
		m_BrickTexture = Lunar::Texture("LunarApp/assets/brick.png");
		m_BrickTexture.LoadTexture();

	// 3. Create Light
		m_MainLight = Lunar::Light(1.0f, 1.0f, 1.0f, 0.4f, 1.0f, -2.0f, -1.0f, 1.0f);

	// 3. create shaders
		m_ShaderProgram = std::make_unique<Lunar::ShaderProgram>(
				"LunarApp/src/shaders/vertex_shader.glsl",
				"LunarApp/src/shaders/fragment_shader.glsl");

	// 3. Init Camera
		const auto& app = Lunar::Application::Get();
		auto width = app.getWindowData().BufferWidth;
		auto height = app.getWindowData().BufferHeight;
		auto aspectRatio = (float)width / (float)height;
		m_EditorCamera = Lunar::EditorCamera(45.0f, aspectRatio, 0.1f, 100.0f);

	}

	// called every render loop
	void OnUpdate(float ts) override
	{
	// Clear window to black.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// 0. Set shader program for single frame parallel rendering.
	// 1. update camera
		m_EditorCamera.OnUpdate(ts);
	// 2. Set View, Projection Matrix (from Editor Camera)
		glUseProgram(m_ShaderProgram->GetProgramID());
		m_ShaderProgram->SetUniformProjection(glm::value_ptr(m_EditorCamera.GetProjection()));
		m_ShaderProgram->SetUniformView(glm::value_ptr(m_EditorCamera.GetViewMatrix()));
		glm::mat4 model(1.0f); // init unit matrix
		m_ShaderProgram->SetUniformModel(glm::value_ptr(model));
	// 3. Bind texture to fragment shader
		m_BrickTexture.UseTexture();
	// 4. Use Light
		m_MainLight.UseLight(m_ShaderProgram->GetUniformAmbientIntensityLocation(), m_ShaderProgram->GetUniformAmbientColorLocation(),
							 m_ShaderProgram->GetUniformDiffuseIntensityLocation(), m_ShaderProgram->GetUniformDirectionLocation());

//		m_Model.RenderModel();
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
	Lunar::ApplicationSpecification spec {"Scoop", 512, 512 };

	auto* app = new Lunar::Application(spec);
    app->PushLayer<ExampleLayer>();
	return (app);
}
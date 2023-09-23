//
// Created by Minkyeu Kim on 9/23/23.
//

#include "RayTracer.h"

RayTracer::RayTracer(const std::shared_ptr<Lunar::AABBTree>& aabbScene, const Lunar::EditorCamera& camera)
		: m_ActiveAABBScene(aabbScene), m_ActiveEditorCamera(&camera)
{}

void RayTracer::SetAABBScene(const std::shared_ptr<Lunar::AABBTree>& aabbScene)
{
	m_ActiveAABBScene = aabbScene;
}

void RayTracer::OnResize(uint32_t width, uint32_t height)
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

void RayTracer::Render(const Lunar::EditorCamera& camera)
{
	Lunar::Timer timer;
	m_ActiveEditorCamera = &camera;

#if (MT == 1) // NOTE: this variable is set by CMakelist.txt

	std::vector<std::future<void>> futures;
	futures.reserve(m_ImageColumnIterator.size());

	// lamda function
	auto DrawEachRow = [this](uint32_t y) -> void
	{
	  for (unsigned int x : m_ImageRowIterator)
	  {
		  glm::vec4 color = _CalculateColorPerPixel(x, y);
		  m_ImageData[(x) + ((y) * m_FinalImageFrameBuffer->GetWidth())] = Utils::ConvertToRGBA(color);
	  }
	};

	// render row
	for (unsigned int y : m_ImageColumnIterator)
	{
		futures.push_back(m_ThreadPool.AddTask(DrawEachRow, y));
	}

	//		// wait until all tasks all done
	std::for_each(futures.begin(), futures.end(), [](std::future<void> &future) -> void
	{
	  future.wait();
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

Lunar::Ray RayTracer::ConvertPixelPositionToWorldSpaceRay(uint32_t pixelX, uint32_t pixelY)
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
	return Lunar::Ray {m_ActiveEditorCamera->GetPosition(), ray_WORLD_DIR };
}

Lunar::Hit RayTracer::TraceRay(const Lunar::Ray& ray)
{
	Lunar::Hit hitResult = m_ActiveAABBScene->IntersectBVH(ray, (int)m_ChangeIntersection);
	return hitResult;
}

glm::vec4 RayTracer::_GetPhongShadedColor(Lunar::Hit hit)
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

glm::vec4 RayTracer::_CalculateColorPerPixel(uint32_t x, uint32_t y)
{
	Lunar::Ray ray = ConvertPixelPositionToWorldSpaceRay(x, y);
	Lunar::Hit hit = TraceRay(ray);
	if (hit.distance < 0.0f)
		return glm::vec4(0.0f); // BLACK
	else
		return _GetPhongShadedColor(hit);
}
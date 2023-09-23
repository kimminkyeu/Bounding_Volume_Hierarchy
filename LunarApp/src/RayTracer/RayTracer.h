//
// Created by Minkyeu Kim on 9/23/23.
//

#ifndef SCOOP_RAYTRACER_H
#define SCOOP_RAYTRACER_H

#include "glm/glm.hpp"

#include "Lunar/Camera/EditorCamera.h"
#include "Lunar/Core/Timer.h"
#include "Lunar/Core/Log.h"
#include "Lunar/FrameBuffer/FrameBuffer.h"
#include "Lunar/Light/Light.h"
#include "Lunar/Material/Material.h"
#include "Lunar/AABB/AABB.h"
#include "Lunar/Thread/ThreadPool.h"

#include "LunarApp/src/Utils/Utils.h"

// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Renderer.cpp
class RayTracer
{

public:
	RayTracer() = default;
	RayTracer(const std::shared_ptr<Lunar::AABBTree>& aabbScene, const std::shared_ptr<Lunar::EditorCamera>& camera);
	RayTracer& operator=(const RayTracer& other) = delete;

	// set camera, aabb scene, and framebuffer
	void Init(const std::shared_ptr<Lunar::AABBTree>& aabbScene, const std::shared_ptr<Lunar::EditorCamera>& camera, uint32_t frameBufferWidth, uint32_t frameBufferHeight);

	void OnResize(uint32_t width, uint32_t height);

	void Render();

	inline std::shared_ptr<Lunar::FrameBuffer> GetFinalImageFrameBuffer() const { return m_FinalImageFrameBuffer; }


public:
	// Converting screen coordinate to world space Ray
	Lunar::Ray ConvertPixelPositionToWorldSpaceRay(uint32_t pixelX, uint32_t pixelY);
	// trace ray using AABB scene
	Lunar::Hit TraceRay(const Lunar::Ray& ray);

public:
	float m_LastRenderTime = 0.0f;
	bool m_ChangeIntersection = false;

private: // private method
	glm::vec4 _GetPhongShadedColor(Lunar::Hit hit);
	glm::vec4 _CalculateColorPerPixel(uint32_t x, uint32_t y);

private: // private data member
	uint32_t* m_ImageData = nullptr;                            // ray-tracing render buffer
	std::shared_ptr<Lunar::FrameBuffer> m_FinalImageFrameBuffer;// framebuffer

	Lunar::Light m_MainLight = { glm::vec3(2.0f, -1.0f, -1.0f), 0.1f, 0.7f, 0.5f };
	Lunar::Material m_Material;

	std::shared_ptr<Lunar::AABBTree> m_ActiveAABBScene = nullptr;
	std::shared_ptr<Lunar::EditorCamera> m_ActiveCamera = nullptr;

	ThreadPool m_ThreadPool; // for Thread Pool
	std::vector<uint32_t> m_ImageColumnIterator, m_ImageRowIterator;
};

#endif//SCOOP_RAYTRACER_H

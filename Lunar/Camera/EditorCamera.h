//
// Created by USER on 2023-07-24.
//

#ifndef SCOOP_EDITORCAMERA_H
#define SCOOP_EDITORCAMERA_H

#include "CameraBase.h"

// About Hazel Editor Camera
	// https://www.youtube.com/watch?v=8zVtRpBTwe0

// Entity Component System
	// https://www.youtube.com/watch?v=Z-CILn2w9K0&list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT&index=76
	// https://www.youtube.com/watch?v=D4hz0wEB978

// Camera System
	// https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/Renderer/EditorCamera.h
	// https://www.youtube.com/watch?v=lXlXqUEEJ94&t=655s
	// https://www.youtube.com/watch?v=q7rwMoZQSmw

// RayTracing in CPU, Camera System Hint
	// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/Camera.h

// 어떻게 RT 구조를 짯는지 보니까... 소스 보니까 image를 화면에 찍는 방식으로 레이트레이싱을 했네.
// 걍 이미지 하나를 그리고, 그걸 ImGui::Image 에다가 때려 박았네...
	// https://github.com/TheCherno/RayTracing/blob/master/RayTracing/src/WalnutApp.cpp#L120 --> 여기 참고 !

// ************************************************************
// Understanding Quaternions!
	// https://www.youtube.com/watch?v=zjMuIxRvygQ
	// https://www.youtube.com/watch?v=mHVwd8gYLnI&t=56s
// ************************************************************

namespace Lunar {

	class EditorCamera : public CameraBase
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(float ts);

		// recalculate on window resize;
		void OnResize(float width, float height);

		inline float GetDistance() const { return m_Distance; }
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); }

		inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		inline glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		inline const glm::vec3& GetPosition() const { return m_Position; }
		glm::quat GetOrientation() const;

		inline float GetPitch() const { return m_Pitch; }
		inline float GetYaw() const { return m_Yaw; }

	private:
		void UpdateProjection();
		void UpdateView();

//		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;

	};
};



#endif //SCOOP_EDITORCAMERA_H

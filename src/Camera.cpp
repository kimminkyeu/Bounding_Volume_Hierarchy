//
// Created by USER on 2023-07-20.
//

#include "Camera.h"

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch,
			   GLfloat startMoveSpeed, GLfloat startRotateSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	movementSpeed = startMoveSpeed;
	turnSpeed = startRotateSpeed;
	this->update();
}

Camera::~Camera()
{}

void Camera::keyControl(bool* keys)
{
	if(keys[GLFW_KEY_W])
	{
		position += front * movementSpeed;
	}
	if(keys[GLFW_KEY_D])
	{
		position += right * movementSpeed;
	}
	if(keys[GLFW_KEY_A])
	{
		position -= right * movementSpeed;
	}
	if(keys[GLFW_KEY_S])
	{
		position -= front * movementSpeed;
	}
}

void Camera::update()
{
//	https://math.stackexchange.com/questions/2618527/converting-from-yaw-pitch-roll-to-vector
// 그냥 y축 회전행렬과 x축 회전행렬을 곱한 것임.
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}


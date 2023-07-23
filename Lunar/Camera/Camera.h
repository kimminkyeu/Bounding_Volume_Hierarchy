//
// Created by USER on 2023-07-20.
//

#ifndef SCOOP_CAMERA_H
#define SCOOP_CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <vector>

// Camera Class가 Projection + View를 모두 가지고 있는게 논리적으로 맞는 것 같다!
// 추후 그렇게 수정 진행하자.
class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat startMoveSpeed,
		   GLfloat startRotateSpeed);

	~Camera();

	// update camera geometry
	void update();

	void keyControl(bool* keys);
	glm::mat4 calculateViewMatrix();



private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;
	GLfloat movementSpeed;
	GLfloat turnSpeed;

};


#endif //SCOOP_CAMERA_H

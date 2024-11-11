#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>

#include <glfw3.h>

#include <iostream>

using namespace std;

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	void mouseButtons(int* buttons);
	void scrollControl(GLfloat yChange, GLfloat deltaTime);
	void dragControl(GLfloat xChange, GLfloat yChange, GLfloat deltaTime);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	glm::mat4 calculateViewMatrix();

	void SetPosition(glm::vec3 pos) { position = pos; }
	void SetFront(glm::vec3 fr) { front = fr; }
	void SetUp(glm::vec3 u) { up = u; }
	void SetYaw(GLfloat y) { yaw = y; }
	void SetPitch(GLfloat p) { pitch = p; }

	void SetClampCamera(bool clamp) { clampCamera = clamp; }

	void Debug();

	~Camera();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	bool EnableMovementKeys = false;
	bool ScrollableWindow = false;
	bool DragWindow = false;
	bool clampCamera = false;
	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};


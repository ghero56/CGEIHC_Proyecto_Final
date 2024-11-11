#pragma once
#include "Light.h"

class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	DirectionalLight(GLfloat* color, GLfloat aIntensity, GLfloat dIntensity,
		GLfloat* dir);

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation, GLfloat diffuseIntensityLocation, GLfloat directionLocation);

	void SetDirection(glm::vec3 dir) { direction = dir; }
	glm::vec3 GetDirection() { return direction; }

	~DirectionalLight();

private:
	glm::vec3 direction;
};
#pragma once

#include <glew.h>
#include <glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity);

	void SetAmbientIntensity(GLfloat aIntensity) { ambientIntensity = aIntensity; }

	void SetDiffuseIntensity(GLfloat dIntensity) { diffuseIntensity = dIntensity; }

	void SetColor(glm::vec3 col) { color = col; }
	glm::vec3 GetColor() { return color; }

	~Light();

protected:
	glm::vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;
};
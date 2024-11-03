#include "Light.h"

Light::Light() {
    color = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;
    diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity) {
    color = glm::vec3(red, green, blue);
    ambientIntensity = aIntensity;
    diffuseIntensity = dIntensity;
}

Light::~Light() {}

void Light::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation, GLuint directionLocation) {
    glUniform3f(ambientColorLocation, color.x, color.y, color.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);
    glUniform1f(diffuseIntensityLocation, diffuseIntensity);
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}

glm::vec3 Light::GetDirection() {
	return direction;
}

void Light::SetDirection(glm::vec3 dir) {
	direction = dir;
}

glm::vec3 Light::GetColor() {
	return color;
}

void Light::SetColor(glm::vec3 col) {
	color = col;
}

GLfloat Light::GetAmbientIntensity() {
	return ambientIntensity;
}

void Light::SetAmbientIntensity(GLfloat aIntensity) {
	ambientIntensity = aIntensity;
}

GLfloat Light::GetDiffuseIntensity() {
	return diffuseIntensity;
}

void Light::SetDiffuseIntensity(GLfloat dIntensity) {
	diffuseIntensity = dIntensity;
}

glm::vec3 Light::GetPosition() {
	return position;
}

void Light::SetPosition(glm::vec3 pos) {
	position = pos;
}

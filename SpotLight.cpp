#include "SpotLight.h"

SpotLight::SpotLight() : PointLight() {
    direction = glm::vec3(0.0f, -1.0f, 0.0f);
    edge = 0.0f;
    procEdge = cosf(glm::radians(edge));
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue,
    GLfloat aIntensity, GLfloat dIntensity,
    GLfloat xPos, GLfloat yPos, GLfloat zPos,
    GLfloat xDir, GLfloat yDir, GLfloat zDir,
    GLfloat con, GLfloat lin, GLfloat exp, GLfloat edge)
    : PointLight(red, green, blue, aIntensity, dIntensity, xPos, yPos, zPos, con, lin, exp) {
    direction = glm::vec3(xDir, yDir, zDir);
    this->edge = edge;
    procEdge = cosf(glm::radians(edge));
}

void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
    GLuint diffuseIntensityLocation, GLuint positionLocation,
    GLuint directionLocation, GLuint constantLocation,
    GLuint linearLocation, GLuint exponentLocation, GLuint edgeLocation) {
    PointLight::UseLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation,
        positionLocation, constantLocation, linearLocation, exponentLocation);
    glUniform3f(directionLocation, direction.x, direction.y, direction.z);
    glUniform1f(edgeLocation, procEdge);
}

SpotLight::~SpotLight() {}

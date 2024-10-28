#pragma once
#include <glm/glm.hpp>
#include <glew.h>
#include "PointLight.h"

class SpotLight : public PointLight {
public:
    SpotLight();
    SpotLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat aIntensity, GLfloat dIntensity,
        GLfloat xPos, GLfloat yPos, GLfloat zPos,
        GLfloat xDir, GLfloat yDir, GLfloat zDir,
        GLfloat con, GLfloat lin, GLfloat exp, GLfloat edge);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation,
        GLuint directionLocation, GLuint constantLocation,
        GLuint linearLocation, GLuint exponentLocation, GLuint edgeLocation);

    ~SpotLight();

private:
    glm::vec3 direction;
    GLfloat edge, procEdge;
};

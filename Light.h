#pragma once
#include <glm/glm.hpp>
#include <glew.h>

class Light {
public:
    Light();
    Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity);
    ~Light();

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
        GLuint diffuseIntensityLocation, GLuint directionLocation);

    glm::vec3 color;
    GLfloat ambientIntensity;
    GLfloat diffuseIntensity;

protected:
    glm::vec3 direction;  // Añadir esta línea para DirectionalLight
};

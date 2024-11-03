#pragma once
#include <glm/glm.hpp>
#include <glew.h>
#include "Light.h"

class DirectionalLight : public Light {
public:
    DirectionalLight();
    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat aIntensity, GLfloat dIntensity,
        GLfloat xDir, GLfloat yDir, GLfloat zDir);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
        GLuint diffuseIntensityLocation, GLuint directionLocation);

    ~DirectionalLight();
private:
	
};


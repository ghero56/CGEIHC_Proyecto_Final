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

		glm::vec3 GetColor();
		GLfloat GetAmbientIntensity();
		GLfloat GetDiffuseIntensity();
		glm::vec3 GetDirection();
		void SetColor(glm::vec3 col);
		void SetAmbientIntensity(GLfloat aIntensity);
		void SetDiffuseIntensity(GLfloat dIntensity);
		void SetDirection(glm::vec3 dir);

		glm::vec3 GetPosition();
		void SetPosition(glm::vec3 pos);

    protected:
        glm::vec3 color;
        GLfloat ambientIntensity;
        GLfloat diffuseIntensity;
        glm::vec3 direction;  // Añadir esta línea para DirectionalLight
		glm::vec3 position;  // Añadir esta línea para PointLight
};

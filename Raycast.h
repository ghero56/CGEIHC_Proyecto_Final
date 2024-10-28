#pragma once
#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <iostream>
#include "GameObject.h"

class Raycast
{
	public:
		Raycast();
		~Raycast();

		void CreateRaycast(glm::vec3 origin, glm::vec3 direction, float length);
		void RenderRaycast();
		void ClearRaycast();
		GameObject* GetCollidedObject(glm::vec3 origin, glm::vec3 direction, float length);
	private:

		GLuint VAO, VBO;
		GLsizei indexCount;
};
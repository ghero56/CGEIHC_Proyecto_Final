#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> // glm::vec3
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

enum ShaderType {
	VERTEX_SHADER,
	FRAGMENT_SHADER
};

static GLuint LoadShader(const char* filename, GLenum shader_type) {
	GLuint shader_id = glCreateShader(shader_type);
	std::ifstream file(filename, ios::in);
	if (!file) {
		cout << "Error opening file " << filename << endl;
		return 0;
	}
	string line, source;
	while (getline(file, line)) {
		source += line + "\n";
	}
	file.close();
	const char* source_c = source.c_str();
	glShaderSource(shader_id, 1, &source_c, NULL);
}
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
	cout << "Compiling shader: " << filename << " id: " << shader_id << endl;
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
	glCompileShader(shader_id);

	// comprobamos la compilación
	GLint isCompiled = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &isCompiled);
		
	if (isCompiled == GL_FALSE){
		GLint maxLength = 0;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader_id, maxLength, &maxLength, &errorLog[0]);
		cout << "Error compiling shader: " << filename << " id: " << shader_id << endl;
		cout << &errorLog[0] << endl;
		glDeleteShader(shader_id);
		return 0;
	}

	// no hay errores
	cout << "Compiled shader: " << filename << " id: " << shader_id << endl;
	return shader_id;
}
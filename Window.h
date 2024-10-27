#pragma once

#include <glew.h>
#include <glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>
#include <iostream>

#include <glm.hpp>

using namespace std;

class Window
{
public:
	GLFWwindow* selfWindow;

	Window();
	~Window();

	int Initialize();
	void SwapBuffers() { glfwSwapBuffers(selfWindow); }
	void PollEvents() { glfwPollEvents(); }
	bool ShouldClose() { return glfwWindowShouldClose(selfWindow); }
	void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	int getBufferWidth() { return bufferWidth; }
	int getBufferHeight() { return bufferHeight; }

private:
	int bufferWidth, bufferHeight;    
};
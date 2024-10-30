#pragma once

#include <glew.h>
#include <glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>
#include <iostream>
#include <stdio.h>

#include <glm.hpp>

using namespace std;

class Window
{
public:
	GLFWwindow* selfWindow;

	Window();
	~Window();

	int Initialize(int,int);
	void SwapBuffers() { glfwSwapBuffers(selfWindow); }
	void PollEvents() { glfwPollEvents(); }
	bool ShouldClose() { return glfwWindowShouldClose(selfWindow); }
	void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	int getBufferWidth() { return bufferWidth; }
	int getBufferHeight() { return bufferHeight; }

	void SetBufferWidth(int width) { bufferWidth = width; }
	void SetBufferHeight(int height) { bufferHeight = height; }

	void SetWindow(GLFWwindow* window) { selfWindow = window; }

	bool* GetKeys() { return keys; }
	double GetMouseX() { return mouse_x; }
	double GetMouseY() { return mouse_y; }
	double GetScrollY() { return scroll_y; }

	int* GetMouseButtons() { return mouseButtons; };

private:
	int bufferWidth, bufferHeight;    
	int key, key_action, key_mods, key_scancode;

	double mouse_x, mouse_y, scroll_y;
	double lastX, lastY;
	bool mouseHasMoved = true;

	bool keys[1024];

	int mouseButtons[3];

	void callBacks();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
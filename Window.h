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

	int getMovimiento() { return movimiento; }
	int getDado1() { return dado1; }
	int getDado2() { return dado2; }
	int getAjusteRot() { return ajusteRot; }
	bool getTirando() { return tirando; }

	void setTirando(bool estado) { tirando = estado; }

	void SetWindow(GLFWwindow* window) { selfWindow = window; }

	bool* GetKeys() { return keys; }
	GLfloat GetMouseX();
	GLfloat GetMouseY();
	GLfloat GetScrollY();

	int* GetMouseButtons() { return mouseButtons; };

private:
	int bufferWidth, bufferHeight;    
	int width, height;
	int key, key_action, key_mods, key_scancode;

	int movimiento, dado1, dado2, ajusteRot;
	bool tirando;

	double mouse_x, mouse_y, scroll_y;
	double lastX, lastY;
	bool mouseHasMoved = false;

	bool keys[1024];

	int mouseButtons[3];

	void callBacks();

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
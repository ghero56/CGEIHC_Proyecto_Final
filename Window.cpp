#include "Window.h"


Window::Window() {
	selfWindow = nullptr;
	bufferWidth = 0;
	bufferHeight = 0;
	std::fill(std::begin(keys), std::end(keys), false);
}

Window::~Window() {
	glfwTerminate();
}

int Window::Initialize(int rx, int ry) {
	if (!glfwInit())
	{
		cout << "Fallo en inicializar GLFW" << endl;
		return 1;
	}

	bufferHeight = ry;
	bufferWidth = rx;

	lastX = bufferWidth / 2.0;
	lastY = bufferHeight / 2.0;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	/* Create a windowed mode window and its OpenGL context */
	selfWindow = glfwCreateWindow(rx, ry, "Proyecto Final CGEIHC 2025-1", NULL, NULL);

	if (!selfWindow)
	{
		cout << "Fallo en crearse la ventana con GLFW" << endl;
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(selfWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(selfWindow);

	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(selfWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
	// Asignar valores de la ventana y coordenadas

	//Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(selfWindow, this);

	callBacks();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(selfWindow, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	
}

void Window::callBacks() {
	glfwSetKeyCallback(selfWindow, keyCallback);
	glfwSetCursorPosCallback(selfWindow, cursorPosCallback);
	glfwSetMouseButtonCallback(selfWindow, mouseButtonCallback);
	glfwSetScrollCallback(selfWindow, scrollCallback);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_W && action)
	{
		cout << "W" << endl;
	}
	if (key == GLFW_KEY_S && action)
	{
		cout << "S" << endl;
	}
	if (key == GLFW_KEY_A && action)
	{
		cout << "A" << endl;
	}
	if (key == GLFW_KEY_D && action)
	{
		cout << "D" << endl;
	}
	if (key == GLFW_KEY_Q && action)
	{
		cout << "Q" << endl;
	}
	if (key == GLFW_KEY_E && action)
	{
		cout << "E" << endl;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			// cout << "se presiono la tecla " << key << endl;
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			// cout << "se solto la tecla " << key << endl;
		}
	}

	// Método de crear nuevo GameObject
	// Método de entrar y salir del modo editor
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseHasMoved) {
		theWindow->lastX = xpos;
		theWindow->lastY = ypos;
		theWindow->mouseHasMoved = false;
	}

	// Calcula el cambio en la posición del mouse
	theWindow->mouse_x = xpos - theWindow->lastX;
	theWindow->mouse_y = theWindow->lastY - ypos;

	// Ahora actualiza `lastX` y `lastY`
	theWindow->lastX = xpos;
	theWindow->lastY = ypos;
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	theWindow->mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = 0;
	theWindow->mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE] = 0;
	theWindow->mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		theWindow->mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = 1;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		theWindow->mouseButtons[GLFW_MOUSE_BUTTON_RIGHT] = 0;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		theWindow->mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 1;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		theWindow->mouseButtons[GLFW_MOUSE_BUTTON_LEFT] = 0;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		theWindow->mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE] = 1;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
	{
		theWindow->mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE] = 0;
	}
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	theWindow->scroll_y = yoffset;
}

GLfloat Window::GetMouseX() {
	GLfloat TNX = mouse_x;
	mouse_x = 0.0f;
	return TNX;
}

GLfloat Window::GetMouseY() {
	GLfloat TNY = mouse_y;
	mouse_y = 0.0f;
	return TNY;
}

GLfloat Window::GetScrollY() {
	GLfloat TNY = scroll_y;
	scroll_y = 0.0f;
	return TNY;
}
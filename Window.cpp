#include "Window.h"


Window::Window() {
	selfWindow = nullptr;
	bufferWidth = 0;
	bufferHeight = 0;
	std::fill(std::begin(keys), std::end(keys), false);
}

int Window::Initialize(int rx, int ry) {
	width = rx;
	height = ry;
	movimiento = 1;
	dado1 = 0;
	dado2 = 0;
	ajusteRot = 0;
	tirando = false;
	if (!glfwInit())
	{
		cout << "Fallo en inicializar GLFW" << endl;
		return 1;
	}

	lastX = width/ 2.0;
	lastY = height / 2.0;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	/* Create a windowed mode window and its OpenGL context */
	selfWindow = glfwCreateWindow(width, height, "Proyecto Final CGEIHC 2025-1", NULL, NULL);

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

	callBacks();

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



	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_Y && action == GLFW_RELEASE)
	{
		if (theWindow->tirando == false)
		{
			theWindow->tirando = true;
			theWindow->dado1 = 1 + std::rand() % 4;
			theWindow->dado2 = 1 + std::rand() % 8;
			theWindow->movimiento += theWindow->dado1 + theWindow->dado2;
			std::cout << "Dado 1: " << theWindow->dado1 << std::endl;
			std::cout << "Dado 2: " << theWindow->dado2 << std::endl;
			std::cout << "Estado: " << theWindow->tirando << std::endl;
			if (theWindow->movimiento > 40)
			{
				theWindow->movimiento -= 40;
				theWindow->ajusteRot += 360;
			}
		}
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

Window::~Window() {
	glfwTerminate();
}
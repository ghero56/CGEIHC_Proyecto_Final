#include "Window.h"


Window::Window() {
	selfWindow = nullptr;
	bufferWidth = 0;
	bufferHeight = 0;
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(selfWindow, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}
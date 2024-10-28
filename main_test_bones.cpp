#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>

#include <iostream>
#include <vector>
#include <string>

#include <fmod.hpp>

#include <imgui.h>
#include "imfilebrowser.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <assimp/Importer.hpp>

#include "Window.h"

// TESTING

#include "COMMODO_VALUES.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AssimpGLMHelpers.h"
#include "GLSL_ShaderCompiler.h"
#include "Mesh.h"

using namespace std;

Window window;
// vector<Shader> shaders;

void NewFrame()
{
	glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void EndOfFrame(GLFWwindow* wind)
{
	glDisable(GL_BLEND);

	// glUseProgram(0);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	/* Swap front and back buffers */
	glfwSwapBuffers(wind);
}

void ExitCleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//glDeleteFramebuffers(1, &FBO);
	//glDeleteTextures(1, &texture_id);
	//glDeleteRenderbuffers(1, &RBO);

	glfwDestroyWindow(window.selfWindow);
	glfwTerminate();
}

int main(void)
{
	// FMOD initialization
	FMOD_RESULT result;
	FMOD::System* system = nullptr;

	result = FMOD::System_Create(&system);

	system->init(512, FMOD_INIT_NORMAL, 0);
	FMOD::Sound* sound = nullptr;
	// system->createSound("assets/sounds/chilango.mp3", FMOD_DEFAULT, 0, &sound);

	FMOD::Channel* channel = nullptr;
	result = system->playSound(sound, nullptr, false, &channel);

	// GLFW initialization
	window = Window();
    window.Initialize(1920, 1080);
	
	// ImGui input output
    ImGuiIO& imgui_io = ImGui::GetIO(); (void)imgui_io;
	imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	/*Also testing*/
	// shaders compilation
	GLuint vertexShader = LoadShader("Assets/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	GLuint fragmentShader = LoadShader("Assets/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// uniform initialization
	// Usar el programa de shaders antes de setear uniformes
	glUseProgram(shaderProgram);

	// Inicialización de uniforms
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	GLuint lightSpaceMatrixLoc = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
	GLuint boneTransformsLoc = glGetUniformLocation(shaderProgram, "boneTransforms");
	GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
	GLuint toffsetLoc = glGetUniformLocation(shaderProgram, "toffset");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 1000.0f);
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

	// Verifica el programa
	GLint success;
	GLchar infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		printf("Error al enlazar el programa de shaders: %s\n", infoLog);
	}

	// eliminar los shaders ahora que están vinculados al programa
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("Assets/Models/aurora.glb", ASSIMP_LOAD_FLAGS);

	GLuint	uniformProjection = 0,
			uniformModel = 0,
			uniformView = 0,
			uniformEyePosition = 0,
			uniformSpecularIntensity = 0,
			uniformShininess = 0,
			uniformTextureOffset = 0,
			uniformColor = 0;

	// imgui
	ImGui::FileBrowser fileDialog;
		// (optional) set browser properties
	fileDialog.SetTitle("title");
	fileDialog.SetTypeFilters({ ".h", ".cpp" });
	
	/*fin Also testing*/

    bool demoWindow;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.selfWindow))
    {
		NewFrame();
		
		// mi código en loop
		

		ImGui::Begin("Titulo impresionante");
		ImGui::Text("Some useful text.");
		ImGui::Checkbox("Demo Window", &demoWindow);
        ImGui::End();
        if (demoWindow)
            ImGui::ShowDemoWindow();
		
		// testing
		// let's draw a triangle
		glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(-0.5f, -0.5f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(0.5f, -0.5f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2f(0.0f, 0.5f);
		glEnd();

		if(ImGui::Begin("Project Explorer"))
		{
			// open file dialog when user clicks this button
			if (ImGui::Button("open file dialog"))
				fileDialog.Open();
		}
		ImGui::End();

		fileDialog.Display();

		if (fileDialog.HasSelected())
		{
			std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
			fileDialog.ClearSelected();
		}
		// fin testing

		// debug
		if (imgui_io.WantCaptureMouse)
		{           
			cout << "Mouse is captured by imgui" << endl;
		}

		EndOfFrame(window.selfWindow);
    }

	ExitCleanup();

    return 0;
}
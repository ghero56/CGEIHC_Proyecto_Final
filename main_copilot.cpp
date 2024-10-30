#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fmod.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "imfilebrowser.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Window.h"
#include "GLSL_ShaderCompiler.h"
#include "Mesh.h"
#include "GameObject.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Camera.h"

using namespace std;

vector<Mesh*> meshList;

Window window;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

void NewFrame() {
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    // deltaTime += (now - lastTime) / limitFPS;
    lastTime = now;

    glfwPollEvents();

    glClearColor(0.f, .5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.keyControl(window.GetKeys(), deltaTime);
    camera.mouseControl(window.GetMouseX(), window.GetMouseY());
    camera.scrollControl(window.GetScrollY(), deltaTime);
    camera.mouseButtons(window.GetMouseButtons());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();   
}

void EndOfFrame(GLFWwindow* wind) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
	window.SwapBuffers();
}

void ExitCleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window.selfWindow);
    glfwTerminate();
}

void EditorTools(bool* demoWindow, ImGui::FileBrowser* fileDialog) { 
    // debug
    ImGui::Begin("Bool para el editor");
    ImGui::Text("Este boton activa la demo, por defecto viene en True");
    ImGui::Checkbox("Demo Window", demoWindow);
    ImGui::End();

    // debug
    if (*demoWindow) {
        ImGui::ShowDemoWindow();
    }

	// file explorer
    if (ImGui::Begin("Project Explorer")) {
        if (ImGui::Button("open file dialog"))
            fileDialog->Open();
    }
    ImGui::End();

    fileDialog->Display();

    // handle de los archivos
    if (fileDialog->HasSelected()) {
        cout << "Selected filename" << fileDialog->GetSelected().string() << std::endl;
        fileDialog->ClearSelected();
    }
}

void CreateObjects()
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
        //	x      y      z			u	  v			nx	  ny    nz
            -1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
    };


    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);
}

int main(void) {
    // FMOD initialization
    FMOD_RESULT result;
    FMOD::System* system = nullptr;
    result = FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, 0);
    
    FMOD::Sound* sound = nullptr;
    system->createSound("Assets/Sounds/Howling Abyss.mp3", FMOD_DEFAULT, 0, &sound);
    FMOD::Channel* channel = nullptr;
    result = system->playSound(sound, nullptr, false, &channel);

    // GLFW initialization
    window = Window();
    window.Initialize(720, 720);

    glfwPollEvents();
    glfwSetInputMode(window.selfWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    // ImGui initialization
    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Shader compilation
    GLuint vertexShader = LoadShader("Assets/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    GLuint fragmentShader = LoadShader("Assets/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Verifica el programa
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "Error al compilar el programa de shaders: " << infoLog << endl;
    }

    // Eliminar los shaders ahora que están vinculados al programa
    //glDeleteShader(vertexShader);
    //glDeleteShader(fragmentShader);

    // Usar el programa de shaders antes de setear uniformes
    // glUseProgram(shaderProgram);

    // inicialización de la camara
    camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

    // Inicialización de uniforms
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint lightSpaceMatrixLoc = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
    GLuint boneTransformsLoc = glGetUniformLocation(shaderProgram, "boneTransforms");
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLuint toffsetLoc = glGetUniformLocation(shaderProgram, "toffset");

    glm::mat4 model = glm::mat4(1.0f);
    // glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 1000.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

    // Configura los uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Crear y configurar el GameObject
    GameObject* aurora = new GameObject((char*)"Aurora LOL");
    aurora->CreateMesh("Assets/Models/aurora.glb");
    aurora->SetScale(glm::vec3(.1f, .1f, .1f));

    ImGui::FileBrowser fileDialog;
    bool demoWindow = true; // quitar
    bool EditorMode = true; 

    CreateObjects();

	glfwGetTime();
	glfwSetTime(0.0);

    while (!glfwWindowShouldClose(window.selfWindow)) {
        
        NewFrame();

        if (EditorMode)     
            EditorTools(&demoWindow, &fileDialog);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Renderización del GameObject
        glUseProgram(shaderProgram);
		model = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(aurora->GetModelMatrix()));
        aurora->Render();

		aurora->EditorTools(!EditorMode);

        EndOfFrame(window.selfWindow);
    }

    ExitCleanup();
    return 0;
}

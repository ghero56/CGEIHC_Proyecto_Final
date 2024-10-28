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

using namespace std;

Window window;

void NewFrame() {
    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void EndOfFrame(GLFWwindow* wind) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(wind);
}

void ExitCleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window.selfWindow);
    glfwTerminate();
}

int main(void) {
    // FMOD initialization
    FMOD_RESULT result;
    FMOD::System* system = nullptr;
    result = FMOD::System_Create(&system);
    system->init(512, FMOD_INIT_NORMAL, 0);
    FMOD::Sound* sound = nullptr;
    system->createSound("Assets/Sounds/chilango.mp3", FMOD_DEFAULT, 0, &sound);
    FMOD::Channel* channel = nullptr;
    result = system->playSound(sound, nullptr, false, &channel);

    // GLFW initialization
    window = Window();
    window.Initialize(1920, 1080);

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
        printf("Error al enlazar el programa de shaders: %s\n", infoLog);
    }

    // Eliminar los shaders ahora que están vinculados al programa
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

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
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 100.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

    // Configura los uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Crear y configurar el GameObject
    GameObject aurora;
    aurora.CreateMesh("Assets/Models/aurora.glb");
    //aurora.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    //aurora.SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

    ImGui::FileBrowser fileDialog;

    bool demoWindow;
    while (!glfwWindowShouldClose(window.selfWindow)) {
        NewFrame();

        ImGui::Begin("Titulo impresionante");
        ImGui::Text("Some useful text.");
        ImGui::Checkbox("Demo Window", &demoWindow);
        ImGui::End();

        if (demoWindow)
            ImGui::ShowDemoWindow();

        if (ImGui::Begin("Project Explorer")) {
            if (ImGui::Button("open file dialog"))
                fileDialog.Open();
        }
        ImGui::End();
        fileDialog.Display();

        if (fileDialog.HasSelected()) {
            std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
            fileDialog.ClearSelected();
        }

        // Renderización del GameObject
        glUseProgram(shaderProgram); // Asegúrate de usar el programa de shaders antes de renderizar
        aurora.Render();

        EndOfFrame(window.selfWindow);
    }

    ExitCleanup();
    return 0;
}

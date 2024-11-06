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
#include <json.hpp>

#include <cereal/archives/json.hpp>
#include<fstream>

using namespace std;

Window window;
Camera camera;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

vector<GameObject*> gameObjects;
vector<Mesh*> meshList;

static char name[20] = "";

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

void SerializeObjects() {
    if (serialize) {
        std::ofstream stream("./Assets/scene.json");
        cereal::BinaryOutputArchive oarchive(stream);

        for (GameObject* obj : gameObjects) {
            oarchive(obj);
        }
    }
    else {
    }
}

void ExitCleanup() {
    SerializeObjects();
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
        ImGui::InputTextWithHint("Nombre", "texto", name, IM_ARRAYSIZE(name));
        if (ImGui::Button("explorador"))
        {
            fileDialog->Open();
        }
    }
    ImGui::End();

    ImGui::Begin("Bool para el editor");
    ImGui::Text("Serializar objetos o no");
    ImGui::Checkbox("serialize", &serialize);
    ImGui::End();



    fileDialog->Display();

    // handle de los archivos
    if (fileDialog->HasSelected())
    {
        cout << "Selected filename" << fileDialog->GetSelected().string() << std::endl;
        char* nombre = _strdup(name);
        gameObjects.push_back(new GameObject(nombre));
        gameObjects[gameObjects.size() - 1]->CreateMesh(fileDialog->GetSelected().string().c_str());
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
    GLuint fragmentShader = LoadShader("Assets/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
    GLuint vertexShader = LoadShader("Assets/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, vertexShader);
    
    glLinkProgram(shaderProgram);

    // Verifica el programa
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cout << "Error al compilar el programa de shaders: " << infoLog << endl;
    }

    // Eliminar los shaders ahora que est�n vinculados al programa
    //glDeleteShader(vertexShader);
    //glDeleteShader(fragmentShader);

    // Usar el programa de shaders antes de setear uniformes
    // glUseProgram(shaderProgram);

    // inicializaci�n de la camara
    camera = Camera(glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), 990.5f, -18.0f, 10.0f, 0.5f);

    // Inicializaci�n de uniforms
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint lightSpaceMatrixLoc = glGetUniformLocation(shaderProgram, "lightSpaceMatrix");
    GLuint boneTransformsLoc = glGetUniformLocation(shaderProgram, "boneTransforms");
    GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
    GLuint toffsetLoc = glGetUniformLocation(shaderProgram, "toffset");



	// Directional Light
    GLuint dirAmbientIntensityLoc = glGetUniformLocation(shaderProgram, "directionalLight.base.ambientIntensity");
    GLuint dirColorLoc = glGetUniformLocation(shaderProgram, "directionalLight.base.color");
    GLuint dirDiffuseIntensityLoc = glGetUniformLocation(shaderProgram, "directionalLight.base.diffuseIntensity");
    GLuint dirDirectionLoc = glGetUniformLocation(shaderProgram, "directionalLight.direction");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 10000.0f);
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

    // Configura los uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Crear y configurar el GameObject
    GameObject* tablero = new GameObject((char*)"Tablero");
    tablero->CreateMesh("Assets/Models/Tablero/tablero.obj");
    
	GameObject* aurora = new GameObject((char*)"Aurora");
	aurora->CreateMesh("Assets/Models/Aurora/aurora.obj");


	GameObject* pointLight = new GameObject((char*)"Point Light", new PointLight(
		1.0f, 1.0f, 1.0f,
        1.0f, 0.1f,
        0.0f, 0.5f, 0.0f,
		1.0f, 1.0f, 1.0f
    ));

	GameObject* spotLight = new GameObject((char*)"Spot Light", new SpotLight(
		1.0f, 1.0f, 1.0f,
		1.0f, 0.1f,
		0.0f, 0.5f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 20.0f
	));

	GameObject* directionalLight = new GameObject((char*)"Directional Light", new DirectionalLight(
		1.0f, 1.0f, 1.0f,
		1.0f, 0.1f,
		0.0f, -1.0f, 0.0f
	));

    ImGui::FileBrowser fileDialog;
    bool demoWindow = true; // quitar
    bool EditorMode = true; 

    CreateObjects();

	glfwGetTime();
	glfwSetTime(0.0);
    
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window.selfWindow)) {
        
        NewFrame();
        
        
        glUseProgram(shaderProgram);
        glActiveTexture(GL_TEXTURE0);

        if (EditorMode)     
            EditorTools(&demoWindow, &fileDialog);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        
        glUniform3fv(colorLoc, 1, glm::value_ptr(color));
        // Directional Light
		directionalLight->UseLight(dirAmbientIntensityLoc, dirColorLoc, dirDiffuseIntensityLoc, dirDirectionLoc);
        pointLight->UseLight(dirAmbientIntensityLoc, dirColorLoc, dirDiffuseIntensityLoc, dirDirectionLoc);
		spotLight->UseLight(dirAmbientIntensityLoc, dirColorLoc, dirDiffuseIntensityLoc, dirDirectionLoc);
        
		/*model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        meshList[0]->RenderMesh();*/

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(aurora->GetModelMatrix()));
		aurora->Render();
		aurora->EditorTools(!EditorMode);

		// Renderizaci�n del GameObject
        /*if (aurora->HasAnimation()) {
            aurora->Animate(deltaTime);
            glUniformMatrix4fv(boneTransformsLoc, aurora->GetBoneTransforms().size(), GL_FALSE, glm::value_ptr(aurora->GetBoneTransforms()[0]));
        }*/


        for (GameObject* objeto : gameObjects) {
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objeto->GetModelMatrix()));
            objeto->Render();
            objeto->EditorTools(!EditorMode);
        }

        if (!gameObjects.empty())
        {
            for (auto& gameObject : gameObjects)
            {
                // Actualizar la matriz de modelo en el shader
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gameObject->GetModelMatrix()));

                // Renderizar el objeto
                gameObject->Render();

                // Herramientas de edici�n (pasando el modo de edici�n como par�metro)
                gameObject->EditorTools(!EditorMode);
            }
        }

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tablero->GetModelMatrix()));
        tablero->Render();
        tablero->EditorTools(!EditorMode);

        EndOfFrame(window.selfWindow);
    }
    //aurora->Serialize();
    ExitCleanup();
    return 0;
}

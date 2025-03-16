//-------------------- Headers --------------------//
// OpenGL 4.6 with GLEW and GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++ Libraries
#include <iostream>
#include <vector>
#include <string>

// FMOD
#include <fmod.hpp>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "imfilebrowser.h"

// Assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// creación de ventana y shaders
#include "Window.h"
#include "GLSL_ShaderCompiler.h"

// Modelos y texturas
#include "Mesh.h"
#include "GameObject.h"

// Iluminación
#include "Skybox.h"
// #include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Camera.h"

// serialización
#include <json.hpp>
#include <fstream>

#include "SceneManager.h"

using namespace std;
using namespace jsoncons;

//  Constante lerp
const float lerpFactor = 0.03f;

//-------------------- Variables globales --------------------//
Window window;
Skybox skybox;
Camera camera;

// Delta time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// GameObjects
vector<GameObject*> gameObjects;
vector<Mesh*> meshList;

// Shaders
vector<Shader> shaderList;
static const char* vShader = "Assets/Shaders/vertexShader.glsl";
static const char* fShader = "Assets/Shaders/fragmentShader.glsl";

//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

// para la creación del nuevo objeto en el editor
static char name[20] = "";

// para serialización correcta
int posis = 0;

// para la animación
int casilla = 20;
int posisArreglo=0;
bool vertical = true;
bool girar = false;
GLfloat tiempo;

// Movimiento Avatar
int casillaAct = 1;
int casillaNueva = 1;
int resultadoDado = 0;
float movAshX = 0.0f;
float movAshZ = 0.0f;
float rotAshY = 0.0f;
float destMovAshX = movAshX;
float destMovAshZ = movAshZ;
float destRotAshY = rotAshY;

// Movimiento Dados
float movDado1Y = 38.0f;
float movDado1YOffset = 15.0f;
float rotDado1Y = 0.0f;
float rotDado1XZ = 0.0f;
float rotDado1YOffset = 2.0f;

float movDado2Y = 38.0f;
float movDado2YOffset = 15.0f;
float rotDado2Y = 0.0f;
float rotDado2XZ = 0.0f;
float rotDado2YOffset = 8.0f;
bool esperando = false;

// ------------------- Funciones ------------------- //
void NewFrame( ) {
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    // deltaTime += (now - lastTime) / limitFPS;
    lastTime = now;

    glfwPollEvents();

    glClearColor(0.f, .0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // handling de los controles
	GLfloat mouse_x = window.GetMouseX();
	GLfloat mouse_y = window.GetMouseY();
    camera.mouseControl(mouse_x, mouse_y);
    camera.dragControl(mouse_x, mouse_y, deltaTime);
    camera.mouseButtons(window.GetMouseButtons());
    camera.scrollControl(window.GetScrollY(), deltaTime);
    camera.keyControl(window.GetKeys(), deltaTime);

	// render de ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();   
}

void EndOfFrame(GLFWwindow* wind) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
	window.SwapBuffers();
}

void Serialize() {
    std::ofstream serialFile("./Assets/scene.json", std::ios_base::out | std::ios_base::trunc);
    assert(serialFile);
    serialFile << "[";
    serialFile.close();
    for (auto& obj : gameObjects) {
        obj->Serialize(posis);
        posis++;
    }
    serialFile.open("./Assets/scene.json", std::ios_base::out | std::ios_base::app);
    serialFile << "]";
    serialFile.close();
}

void Decode() {
    std::ifstream is("./Assets/scene.json");

    json_stream_cursor cursor(is);
    cursor.next();
    const auto& event = cursor.current();   
    
    int posisI = 0;

    for (; !cursor.done(); cursor.next())
    {
        if (cursor.current().event_type() == staj_event_type::begin_object) {
            
            GameObject* obj = gameObjects[posisI];

            glm::vec3 posis;
            glm::vec3 rota;
            glm::vec3 scal;
            glm::mat4 model;

            cursor.next();
            cursor.next();
            //obj->setName(cursor.current().get<std::string_view>());
            cursor.next();
            cursor.next();
            cursor.next();
            cursor.next();

            model[0].x = (float)cursor.current().get<double>();
            cursor.next();
            model[0].y = (float)cursor.current().get<double>();
            cursor.next();
            model[0].z = (float)cursor.current().get<double>();

            cursor.next();
            cursor.next();
            cursor.next();

            model[1].x = (float)cursor.current().get<double>();
            cursor.next();
            model[1].y = (float)cursor.current().get<double>();
            cursor.next();
            model[1].z = (float)cursor.current().get<double>();

            cursor.next();
            cursor.next();
            cursor.next();

            model[2].x = (float)cursor.current().get<double>();
            cursor.next();
            model[2].y = (float)cursor.current().get<double>();
            cursor.next();
            model[2].z = (float)cursor.current().get<double>();

            cursor.next();
            cursor.next();
            cursor.next();

            model[3].x = (float)cursor.current().get<double>();
            cursor.next();
            model[3].y = (float)cursor.current().get<double>();
            cursor.next();
            model[3].z = (float)cursor.current().get<double>();

            obj->SetModelMatrix(model);

            cursor.next();
            cursor.next();
            cursor.next();
            cursor.next();
            cursor.next();

            posis.x = (float)cursor.current().get<double>();
            cursor.next();
            posis.y = (float)cursor.current().get<double>();
            cursor.next();
            posis.z = (float)cursor.current().get<double>();

            obj->SetPosition(posis);

            cursor.next();
            cursor.next();
            cursor.next();
            cursor.next();

            rota.x = (float)cursor.current().get<double>();
            cursor.next();
            rota.y = (float)cursor.current().get<double>();
            cursor.next();
            rota.z = (float)cursor.current().get<double>();

            obj->SetRotation(rota);

            cursor.next();
            cursor.next();
            cursor.next();
            cursor.next();

            scal.x = (float)cursor.current().get<double>();
            cursor.next();
            scal.y = (float)cursor.current().get<double>();
            cursor.next();
            scal.z = (float)cursor.current().get<double>();

            obj->SetScale(scal);
            posisI++;

        }
    }

}

void ExitCleanup() {
    Serialize();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window.selfWindow);
    glfwTerminate();
}

void EditorTools(ImGui::FileBrowser* fileDialog) { 	

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

bool keyGPressed = false;

void HandleChangeGameMode(bool* EditorMode, bool* SpaceMode, bool* GameMode) {
    if (window.GetKeys()[GLFW_KEY_LEFT_CONTROL] && window.GetKeys()[GLFW_KEY_G]) {
        if (!keyGPressed) {
            if (*GameMode) { *EditorMode = true; *SpaceMode = false; *GameMode = false; }
            else if (*SpaceMode) { *EditorMode = false; *SpaceMode = false; *GameMode = true; }
            else { *EditorMode = false; *SpaceMode = true; *GameMode = false; }
            keyGPressed = true;
        }
    } else keyGPressed = false;
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

void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

void CreateSkybox() {
    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Assets/Textures/Skybox/cupertin-lake_rt.tga");
    skyboxFaces.push_back("Assets/Textures/Skybox/cupertin-lake_lf.tga");
    skyboxFaces.push_back("Assets/Textures/Skybox/cupertin-lake_dn.tga");
    skyboxFaces.push_back("Assets/Textures/Skybox/cupertin-lake_up.tga");
    skyboxFaces.push_back("Assets/Textures/Skybox/cupertin-lake_bk.tga");
    skyboxFaces.push_back("Assets/Textures/Skybox/cupertin-lake_ft.tga");

    skybox = Skybox(skyboxFaces);
}

void CreateLights() {
    pointLights[pointLightCount++] = PointLight(
        1.0f, 0.0f, 0.0f,
        .70f, .70f,
        10.0f, 1.0f, 10.0f,
        1.0f, 0.1f, .0f
    );
    spotLights[spotLightCount++] = SpotLight(
        0.0f, 1.0f, 1.0f,
        1.f, 1.f,
        90.0f, 5.f, 90.0f,
        0.0f, -1.0f, 0.0f,
        1.f, 0.2f, 0.1f, 100.0f
    );
    spotLights[spotLightCount++] = SpotLight(
        1.0f, 0.0f, 1.0f,
        1.f, 1.f,
        -90.0f, 5.f, 90.0f,
        0.0f, -1.0f, 0.0f,
        1.f, 0.2f, 0.1f, 100.0f
    );
    spotLights[spotLightCount++] = SpotLight(
        1.0f, 1.0f, 0.0f,
        1.f, 1.f,
        90.0f, 5.f, -90.0f,
        0.0f, -1.0f, 0.0f,
        1.f, 0.2f, 0.1f, 100.0f
    );
    spotLights[spotLightCount++] = SpotLight(
        0.0f, 0.0f, 1.0f,
        1.f, 1.f,
        -90.0f, 5.f, -90.0f,
        0.0f, -1.0f, 0.0f,
        1.f, 0.2f, 0.0f, 100.0f
    );
}

void DirLightControlers(float* dirLightAmbientIntensity, float* dirLightDiffuseIntensity, float* dirLightColor, float* dirLightDirection) {
	ImGui::Begin("Directional Light");
	ImGui::SliderFloat("Directional Light Ambient Intensity", dirLightAmbientIntensity, 0.0f, 1.0f);
	ImGui::SliderFloat("Directional Light Diffuse Intensity", dirLightDiffuseIntensity, 0.0f, 1.0f);
	ImGui::ColorEdit3("Directional Light Color", dirLightColor);
	ImGui::SliderFloat3("Direction", dirLightDirection, -1.0f, 1.0f);
	ImGui::End();
}

float lerp2(float a, float b, float t)
{
    return a + t * (b - a);
}

void Recorrido()
{
    if (casillaAct == 1)
    {
        destMovAshX = 88.112f;
        destMovAshZ = 86.617f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 2)
    {
        destMovAshX = 88.112f;
        destMovAshZ = 65.96f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 3)
    {
        destMovAshX = 88.112f;
        destMovAshZ = 49.638f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 4)
    {
        destMovAshX = 88.112f;
        destMovAshZ = 32.468f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 5)
    {
        destMovAshX = 88.112f;
        destMovAshZ = 16.171f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 6)
    {
        destMovAshX = 88.112f;
        destMovAshZ = 0.457f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 7)
    {
        destMovAshX = 88.112f;
        destMovAshZ = -14.981f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 8)
    {
        destMovAshX = 88.112f;
        destMovAshZ = -31.539f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 9)
    {
        destMovAshX = 88.112f;
        destMovAshZ = -48.513f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 10)
    {
        destMovAshX = 88.112f;
        destMovAshZ = -64.076f;
        destRotAshY = -90.0f + window.getAjusteRot();
    }
    if (casillaAct == 11)
    {
        destMovAshX = 87.394f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 12)
    {
        destMovAshX = 65.48f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 13)
    {
        destMovAshX = 48.952f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 14)
    {
        destMovAshX = 32.577f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 15)
    {
        destMovAshX = 17.311f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 16)
    {
        destMovAshX = 0.663f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 17)
    {
        destMovAshX = -14.975f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 18)
    {
        destMovAshX = -32.043f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 19)
    {
        destMovAshX = -47.744f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 20)
    {
        destMovAshX = -64.201f;
        destMovAshZ = -86.267f;
        destRotAshY = 0.0f + window.getAjusteRot();
    }
    if (casillaAct == 21)
    {
        destMovAshX = -85.202f;
        destMovAshZ = -86.267f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 22)
    {
        destMovAshX = -85.202f;
        destMovAshZ = -65.178f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 23)
    {
        destMovAshX = -85.202f;
        destMovAshZ = -48.701f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 24)
    {
        destMovAshX = -85.202f;
        destMovAshZ = -32.034f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 25)
    {
        destMovAshX = -85.202f;
        destMovAshZ = -16.777f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 26)
    {
        destMovAshX = -85.202f;
        destMovAshZ = -0.723f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 27)
    {
        destMovAshX = -85.202f;
        destMovAshZ = 15.906f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 28)
    {
        destMovAshX = -85.202f;
        destMovAshZ = 31.338f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 29)
    {
        destMovAshX = -85.202f;
        destMovAshZ = 47.785f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 30)
    {
        destMovAshX = -85.202f;
        destMovAshZ = 63.709f;
        destRotAshY = 90.0f + window.getAjusteRot();
    }
    if (casillaAct == 31)
    {
        destMovAshX = -85.202f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 32)
    {
        destMovAshX = -65.002f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 33)
    {
        destMovAshX = -47.744f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 34)
    {
        destMovAshX = -32.103f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 35)
    {
        destMovAshX = -15.487f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 36)
    {
        destMovAshX = 0.645f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 37)
    {
        destMovAshX = 15.957f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 38)
    {
        destMovAshX = 31.988f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 39)
    {
        destMovAshX = 48.733f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }
    if (casillaAct == 40)
    {
        destMovAshX = 64.346f;
        destMovAshZ = 86.617f;
        destRotAshY = 180.0f + window.getAjusteRot();
    }

    movAshX = lerp2(movAshX, destMovAshX, lerpFactor);
    movAshZ = lerp2(movAshZ, destMovAshZ, lerpFactor);
    rotAshY = lerp2(rotAshY, destRotAshY, lerpFactor);
}

void ManejoDados()
{
    if (window.getTirando() == false && esperando == false)
    {
        movDado1Y = 38.0f;
        rotDado1Y += rotDado1YOffset * deltaTime;
        movDado2Y = 38.0f;
        rotDado2Y += rotDado2YOffset * deltaTime;
    }
    else if (window.getTirando() == true && esperando == true)
    {
        movDado1Y = 38.0f;
        movDado2Y = 38.0f;
        rotDado1Y = 0.0f;
        rotDado2Y = 0.0f;
        esperando = false;
    }
    else if (window.getTirando() == true && esperando == false)
    {
        if (movDado1Y > 7.2f)
        {
            movDado1Y -= movDado1YOffset * deltaTime;
            movDado2Y -= movDado2YOffset * deltaTime;
            rotDado1Y += 500 * rotDado1YOffset * deltaTime;
            rotDado2Y += 500 * rotDado2YOffset * deltaTime;
            rotDado1XZ += 12 * rotDado1YOffset * deltaTime;
            rotDado2XZ += 5 * rotDado2YOffset * deltaTime;
        }
        else
        {
            window.setTirando(false);
            esperando = true;
        }
    }
    else if (window.getTirando() == false && esperando == true)
    {
        rotDado1Y += rotDado1YOffset * deltaTime;
        rotDado2Y += rotDado2YOffset * deltaTime;
        rotDado1XZ = 0.0f;
        rotDado2XZ = 0.0f;
    }

}

// ------------------- Main ------------------- //
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
    window.Initialize(1920, 1080);

    glfwPollEvents();
    glfwSetInputMode(window.selfWindow, GLFW_STICKY_KEYS, GLFW_TRUE);

    // ImGui initialization
    ImGuiIO& imgui_io = ImGui::GetIO();
    imgui_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Shader compilation & uniform locations
	CreateShaders();

    GLuint uniformProjection = 0,
        uniformModel = 0,
        uniformView = 0,
        uniformEyePosition = 0,
        uniformSpecularIntensity = 0,
        uniformShininess = 0,
        uniformColor = 0;

	// inicializaci�n de la camara y la proyecci�n
    camera = Camera(
                        glm::vec3(0.0f, 100.0f, 10.0f), 
                        glm::vec3(0.0f, 1.0f, 0.0f), 
                        990.5f, 
                        -18.0f, 
                        50.0f, 
                        0.5f
    );
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 10000.0f);

    GameObject* tablero = new GameObject((char*)"Tablero");
    gameObjects.push_back(tablero);
    tablero->CreateMesh("Assets/Models/Tablero/tablero.obj");

    GameObject* morgana = new GameObject((char*)"Morgana");
    gameObjects.push_back(morgana);
    morgana->CreateMesh("Assets/Models/Fernando/morgana.obj");

    if (filesystem::exists("./Assets/scene.json"))
    {
        Decode();
    }
    
    CreateSkybox();

    float dirLightColor[] = { 1.0f, 1.0f, 1.0f };
    float dirLightAmbientIntensity = 0.5f;
    float dirLightDiffuseIntensity = 0.5f;
    float dirLightDirection[] = { 0.0f, 1.0f, 0.5f };
    DirectionalLight* directionalLight = new DirectionalLight( dirLightColor, dirLightAmbientIntensity, dirLightDiffuseIntensity, dirLightDirection );

    CreateLights();

    ImGui::FileBrowser fileDialog;
    bool EditorMode = true, SpaceMode = false, GameMode = false; 

    // CreateObjects();

	glfwGetTime();
	glfwSetTime(0.0);

    glm::mat4 model(1.0);
    glm::mat4 modelAsh(1.0);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 posAsh = morgana->GetPosition();
    glm::vec3 rotAsh = morgana->GetRotation();

    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    deltaTime += (now - lastTime) / limitFPS;
    lastTime = now;

    while (!glfwWindowShouldClose(window.selfWindow)) {
        
        NewFrame();
        skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

        HandleChangeGameMode(&EditorMode, &SpaceMode, &GameMode);
        if (keyGPressed) {
            float oldSize = ImGui::GetFont()->Scale;
            ImGui::GetFont()->Scale *= 2.0f;
			ImGui::PushFont(ImGui::GetFont());
            ImGui::OpenPopup("Game Mode", ImGuiWindowFlags_NoTitleBar);

			ImGui::BeginPopupModal("Game Mode", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			if (EditorMode)
				ImGui::Text("Entering Editor Mode");
			else if (SpaceMode)
				ImGui::Text("Entering Space Mode");
			else if (GameMode)
                ImGui::Text("Entering Game Mode");
            ImGui::EndPopup();

            ImGui::GetFont()->Scale = oldSize;
			ImGui::PopFont();
        }
        

        if (EditorMode) {
            EditorTools(&fileDialog);
            DirLightControlers(&dirLightAmbientIntensity, &dirLightDiffuseIntensity, dirLightColor, dirLightDirection);
			camera.SetClampCamera(false);
        } else if (SpaceMode) {
            camera.SetPosition(glm::vec3(0.0f, 300.0f, 10.0f));
			camera.SetFront(glm::vec3(0.0f, -1.0f, 0.0f));
            camera.SetYaw(0.0f);
            camera.SetPitch(0.0f);
			camera.calculateViewMatrix();
        } else if (GameMode) {
			camera.SetPosition(morgana->GetPosition() + morgana->GetCameraOffset()); 
			camera.SetFront(morgana->GetObjectFront());
            camera.SetYaw(0.0f);
            camera.SetPitch(-90.0f);
            camera.SetUp(glm::vec3(0.0f, 1.0f, 0.0f));
            camera.calculateViewMatrix();
			camera.SetClampCamera(true);
		}

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformEyePosition = shaderList[0].GetEyePositionLocation();
        uniformColor = shaderList[0].getColorLocation();

        //Manejo de animaciones
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        //información en el shader de intensidad especular y brillo
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();
        
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        
        // Directional Light
		directionalLight->SetColor(glm::vec3(dirLightColor[0], dirLightColor[1], dirLightColor[2]));
		directionalLight->SetAmbientIntensity(dirLightAmbientIntensity);
		directionalLight->SetDiffuseIntensity(dirLightDiffuseIntensity);
		directionalLight->SetDirection(glm::vec3(dirLightDirection[0], dirLightDirection[1], dirLightDirection[2]));
        shaderList[0].SetDirectionalLight(directionalLight);

		pointLights[0].SetPosition(morgana->GetPosition() + glm::vec3(.0f,4.0f,0.f));
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);
        
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));

        if (!gameObjects.empty())
        {
            for (auto& gameObject : gameObjects)
            {
                if (posisArreglo == casilla);
                {
                    glm::vec3 movement = gameObject->GetPosition();
                    glm::vec3 rotation = gameObject->GetRotation();

                    if (!girar) {
                        if (vertical) {
                            if (movement.y < 30.0f) {
                                movement.y += 0.3f;
                            }
                            else {
                                vertical = !vertical;
                                tiempo = glfwGetTime();
                            }
                        }
                        else {
                            if (glfwGetTime() > tiempo + 3.0f) {
                                girar = !girar;
                            }
                            else {
                                rotation.y += 5.0f;
                            }
                        }
                    }
                    else {
                        if (movement.y > -30.0f) {
                            movement.y -= 0.3f;
                            rotation.y = 0;
                        }
                        else {
                            casilla = -1;
                        }
                    }
                    gameObject->SetPosition(movement);
                    gameObject->SetRotation(rotation);
                }

                // Actualizar la matriz de modelo en el shader
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(gameObject->GetModelMatrix()));
                //gameObjects.push_back(gameObject);
                
                // Renderizar el objeto
                gameObject->Render();
                
                // Herramientas de ediciï¿½n (pasando el modo de ediciï¿½n como parï¿½metro)
                gameObject->EditorTools(!EditorMode);
                posisArreglo++;
            }
            posisArreglo = 0;
        }

        Recorrido();
        if (esperando == true)
        casilla = window.getMovimiento();

        casillaAct = window.getMovimiento();
        //casilla = casillaAct;
        ManejoDados();

        morgana->SetPosition(glm::vec3(posAsh.x + movAshX, posAsh.y, posAsh.z + movAshZ));
        morgana->SetRotation(glm::vec3(rotAsh.x, rotAsh.y + rotAshY, rotAsh.z));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(morgana->GetModelMatrix()));
        morgana->Render();
        morgana->EditorTools(!EditorMode);

        EndOfFrame(window.selfWindow);
    }

    ExitCleanup();
    return 0;
}

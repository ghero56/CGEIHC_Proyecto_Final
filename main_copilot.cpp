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
#include<fstream>

#include "SceneManager.h"

using namespace std;
using namespace jsoncons;

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
int posis = 0;

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
    window.Initialize(1080, 1080);

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

    GameObject* templo2 = new GameObject((char*)"templo2");
    gameObjects.push_back(templo2);
    templo2->CreateMesh("Assets/Models/Oscar/templo_aire_2.fbx");

    GameObject* pLeague = new GameObject((char*)"Pokemon League");
    gameObjects.push_back(pLeague);
    pLeague->CreateMesh("Assets/Models/Omar/Pokémon League.obj");

    GameObject* mesa_pai_sho = new GameObject((char*)"mesa_pai_sho");
    gameObjects.push_back(mesa_pai_sho);
    mesa_pai_sho->CreateMesh("Assets/Models/Oscar/mesa_pai_sho.obj");

    GameObject* pMansion = new GameObject((char*)"Pokemon Mansion");
    gameObjects.push_back(pMansion);
    pMansion->CreateMesh("Assets/Models/Omar/Pokemon Mansion.obj");

    GameObject* appa = new GameObject((char*)"Appa");
    gameObjects.push_back(appa);
    appa->CreateMesh("Assets/Models/Oscar/appa.fbx");

    GameObject* pArena = new GameObject((char*)"Pokemon Arena");
    gameObjects.push_back(pArena);
    pArena->CreateMesh("Assets/Models/Omar/Mineways2Skfb.obj");


    GameObject* yumi = new GameObject((char*)"Yumi edg");
    gameObjects.push_back(yumi);
    yumi->CreateMesh("Assets/Models/Fernando/edg_yuumi.glb");

    GameObject* morgana = new GameObject((char*)"Morgana");
    gameObjects.push_back(morgana);
    morgana->CreateMesh("Assets/Models/Fernando/morgana.obj");

    GameObject* diana = new GameObject((char*)"diana");
    gameObjects.push_back(diana);
    diana->CreateMesh("Assets/Models/Fernando/winterblessed_diana.glb");

    GameObject* dragonE = new GameObject((char*)"Dragon Elder");
    gameObjects.push_back(dragonE);
    dragonE->CreateMesh("Assets/Models/Fernando/dragon_(elder).glb");

    GameObject* gromp = new GameObject((char*)"Gromp");
    gameObjects.push_back(gromp);
    gromp->CreateMesh("Assets/Models/Fernando/gromp.glb");

    GameObject* akshan = new GameObject((char*)"Akshan");
    gameObjects.push_back(akshan);
    akshan->CreateMesh("Assets/Models/Fernando/cyber_pop_akshan.glb");

    GameObject* maestroJ = new GameObject((char*)"Maestro Jhin");
    gameObjects.push_back(maestroJ);
    akshan->CreateMesh("Assets/Models/Fernando/maestro_jhin.glb");

    GameObject* baronN = new GameObject((char*)"Baron Nashor");
    gameObjects.push_back(baronN);
    akshan->CreateMesh("Assets/Models/Fernando/baron.obj");

    GameObject* pWorld = new GameObject((char*)"Pokemon World");
    gameObjects.push_back(pWorld);
    pWorld->CreateMesh("Assets/Models/Omar/Pokeball Place.fbx");

    GameObject* pHouse = new GameObject((char*)"Pokemon House");
    gameObjects.push_back(pHouse);
    pHouse->CreateMesh("Assets/Models/Omar/pokehouse1.fbx");

    GameObject* submarine = new GameObject((char*)"Aqua Submarine");
    gameObjects.push_back(submarine);
    submarine->CreateMesh("Assets/Models/Omar/submarine.obj");

    GameObject* healer = new GameObject((char*)"Pokemon Healer");
    gameObjects.push_back(healer);
    healer->CreateMesh("Assets/Models/Omar/healer.obj");

    GameObject* movil = new GameObject((char*)"PokeMovile");
    gameObjects.push_back(movil);
    movil->CreateMesh("Assets/Models/Omar/Van21.fbx");

    GameObject* regi = new GameObject((char*)"Regirock");
    gameObjects.push_back(regi);
    regi->CreateMesh("Assets/Models/Omar/Regirock_marmoset.fbx");

    GameObject* pMan2 = new GameObject((char*)"Pokemon Mansion 2");
    gameObjects.push_back(pMan2);
    pMan2->CreateMesh("Assets/Models/Omar/Pokemon League.obj");

    GameObject* pCenter = new GameObject((char*)"Pokemon Center");
    gameObjects.push_back(pCenter);
    pCenter->CreateMesh("Assets/Models/Omar/Pokemon_Center.obj");

    GameObject* pikoM = new GameObject((char*)"Pikomat");
    gameObjects.push_back(pikoM);
    pikoM->CreateMesh("Assets/Models/Omar/Model_1.fbx");

    GameObject* groudon = new GameObject((char*)"Groudon");
    gameObjects.push_back(groudon);
    groudon->CreateMesh("Assets/Models/Omar/Groudon.fbx");

    GameObject* dragon = new GameObject((char*)"dragon");
    gameObjects.push_back(dragon);
    dragon->CreateMesh("Assets/Models/Oscar/dragon.fbx");

    GameObject* glider = new GameObject((char*)"glider");
    gameObjects.push_back(glider);
    glider->CreateMesh("Assets/Models/Oscar/glider.fbx");

    GameObject* ba_sin_se = new GameObject((char*)"ba_sin_se");
    gameObjects.push_back(ba_sin_se);
    ba_sin_se->CreateMesh("Assets/Models/Oscar/ba_sin_se.obj");

    GameObject* tribuA = new GameObject((char*)"tribuA");
    gameObjects.push_back(tribuA);
    tribuA->CreateMesh("Assets/Models/Oscar/tribu_agua.obj");

    GameObject* templo1 = new GameObject((char*)"templo1");
    gameObjects.push_back(templo1);
    templo1->CreateMesh("Assets/Models/Oscar/templo_aire_1.obj");

    GameObject* tetera = new GameObject((char*)"tetera");
    gameObjects.push_back(tetera);
    tetera->CreateMesh("Assets/Models/Oscar/tetera_iroh.fbx");

    GameObject* carro_coles = new GameObject((char*)"carro_coles");
    gameObjects.push_back(carro_coles);
    carro_coles->CreateMesh("Assets/Models/Oscar/carro_coles.fbx");

    GameObject* soldado = new GameObject((char*)"soldado");
    gameObjects.push_back(soldado);
    soldado->CreateMesh("Assets/Models/Oscar/soldado_nf.obj");

    GameObject* momo = new GameObject((char*)"momo");
    gameObjects.push_back(momo);
    momo->CreateMesh("Assets/Models/Oscar/momo.fbx");

    GameObject* prison = new GameObject((char*)"prision");
    gameObjects.push_back(prison);
    prison->CreateMesh("Assets/Models/Oscar/prision.obj");
    
    GameObject* zoe = new GameObject((char*)"Zoe");
    gameObjects.push_back(zoe);
    zoe->CreateMesh("Assets/Models/Fernando/zoe.obj");
    zoe->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));

    if (filesystem::exists("./Assets/scene.json"))
    {
        Decode();
        // Crear y configurar el GameObject
        /*GameObject* tablero = new GameObject((char*)"Tablero");
        gameObjects.push_back(tablero);
        tablero->CreateMesh("Assets/Models/Tablero/tablero.obj");

        GameObject* aurora = new GameObject((char*)"Aurora");
        gameObjects.push_back(aurora);
        aurora->CreateMesh("Assets/Models/Aurora/aurora.obj");*/
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
			camera.SetPosition(zoe->GetPosition() + zoe->GetCameraOffset());
			camera.SetFront(zoe->GetObjectFront());
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

		pointLights[0].SetPosition(zoe->GetPosition() + glm::vec3(.0f,4.0f,0.f));
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);
        
		glm::mat4 model(1.0);
        glm::mat4 modelaux(1.0);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));

        if (!gameObjects.empty())
        {
            for (auto& gameObject : gameObjects)
            {
                // Actualizar la matriz de modelo en el shader
                glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(gameObject->GetModelMatrix()));
                //gameObjects.push_back(gameObject);
                
                // Renderizar el objeto
                gameObject->Render();
                
                // Herramientas de ediciï¿½n (pasando el modo de ediciï¿½n como parï¿½metro)
                gameObject->EditorTools(!EditorMode);
            }
        }

        EndOfFrame(window.selfWindow);
    }

    ExitCleanup();
    return 0;
}

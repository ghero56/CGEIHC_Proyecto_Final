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

#include <cereal/archives/json.hpp>
#include<fstream>

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

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];
unsigned int spotLightCount = 0;
unsigned int pointLightCount = 0;

// para la creación del nuevo objeto en el editor
static char name[20] = "";
int posis = 0;

// ------------------- Funciones ------------------- //
void NewFrame() {
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    // deltaTime += (now - lastTime) / limitFPS;
    lastTime = now;

    glfwPollEvents();

    glClearColor(0.f, .0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // handling de los controles
    camera.mouseControl(window.GetMouseX(), window.GetMouseY());
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

    // Shader compilation

	CreateShaders();


    // inicializaciï¿½n de la camara
    camera = Camera(glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f), 990.5f, -18.0f, 50.0f, 0.5f);


    GLuint  uniformProjection = 0, 
            uniformModel = 0, 
            uniformView = 0,
            uniformEyePosition = 0,
            uniformSpecularIntensity = 0, 
            uniformShininess = 0, 
            uniformColor = 0;


    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 10000.0f);

    GameObject* tablero = new GameObject((char*)"Tablero");
    gameObjects.push_back(tablero);
    tablero->CreateMesh("Assets/Models/Tablero/tablero.obj");
    

    GameObject* templo2 = new GameObject((char*)"templo2");
    gameObjects.push_back(templo2);
    templo2->CreateMesh("Assets/Models/Oscar/templo_aire_2.fbx");

    GameObject* mesa_pai_sho = new GameObject((char*)"mesa_pai_sho");
    gameObjects.push_back(mesa_pai_sho);
    mesa_pai_sho->CreateMesh("Assets/Models/Oscar/mesa_pai_sho.obj");

    GameObject* appa = new GameObject((char*)"Appa");
    gameObjects.push_back(appa);
    appa->CreateMesh("Assets/Models/Oscar/appa.fbx");
    
    
    
    
    GameObject* yumi = new GameObject((char*)"Yumi edg");
    gameObjects.push_back(yumi);
    yumi->CreateMesh("Assets/Models/Fernando/edg_yuumi.glb");
    
    GameObject* morgana = new GameObject((char*)"Morgana");
    gameObjects.push_back(morgana);
    morgana->CreateMesh("Assets/Models/Fernando/morgana.glb");
    
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
    /*
    GameObject* maestroJ = new GameObject((char*)"Maestro Jhin");
    gameObjects.push_back(maestroJ);
    akshan->CreateMesh("Assets/Models/Fernando/maestro_jhin.glb");

    GameObject* baronN = new GameObject((char*)"Baron Nashor");
    gameObjects.push_back(baronN);
    akshan->CreateMesh("Assets/Models/Fernando/baron.glb");
    */

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

	PointLight* pointLight = new PointLight(
		1.0f, 1.0f, 1.0f,
        1.0f, 0.1f,
        0.0f, 0.5f, 0.0f,
		1.0f, 1.0f, 1.0f
    );

	SpotLight* spotLight = new SpotLight(
		1.0f, 1.0f, 1.0f,
		1.0f, 0.1f,
		0.0f, 0.5f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 20.0f
	);

	DirectionalLight* directionalLight = new DirectionalLight(
		1.0f, 1.0f, 1.0f,
		1.0f, 0.1f,
		0.0f, -1.0f, -.5f
	);

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

        skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
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
        
        // borrar, la luz pegada a la cámara
        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

        
        // Directional Light
        shaderList[0].SetDirectionalLight(directionalLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);
        
		glm::mat4 model(1.0);
        glm::mat4 modelaux(1.0);
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        meshList[0]->RenderMesh();




        /*
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(aurora->GetModelMatrix()));
		aurora->Render();
		aurora->EditorTools(!EditorMode);

		
        // Renderizaciï¿½n del GameObject

        if (aurora->HasAnimation()) {
            aurora->Animate(deltaTime);
            glUniformMatrix4fv(boneTransformsLoc, aurora->GetBoneTransforms().size(), GL_FALSE, glm::value_ptr(aurora->GetBoneTransforms()[0]));
        }
        */


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

                /*if (gameObject->HasAnimation()) {
                    gameObject->Animate(deltaTime);
                    glUniformMatrix4fv(boneTransformsLoc, gameObject->GetBoneTransforms().size(), GL_FALSE, glm::value_ptr(gameObject->GetBoneTransforms()[0]));
                }*/
            }
        }


        /*
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tablero->GetModelMatrix()));

        tablero->Render();
        tablero->EditorTools(!EditorMode);
        */
        EndOfFrame(window.selfWindow);
    }
    //aurora->Serialize();
    ExitCleanup();
    return 0;
}

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

void parse_single_bone(int bone_i, const aiBone* bone)
{
	for (int i = 0; i < bone->mNumWeights; i++)
	{
		aiVertexWeight weight = bone->mWeights[i];
		cout << "Bone " << bone_i << " has weight " << weight.mWeight << " on vertex " << weight.mVertexId << endl;
	}
}

void parse_mesh_bones(const aiMesh* pMesh) {
	for (int i = 0; i < pMesh->mNumBones; i++)
	{
		parse_single_bone(i, pMesh->mBones[i]);
	}
}

void parse_meshes(const aiScene* scene)
{
	cout << "Parsing meshes" << endl;

	int total_vertices = 0;
	int total_indices = 0;
	int total_bones = 0;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		int num_vertices = mesh->mNumVertices;
		int num_indices = mesh->mNumFaces * 3;
		int num_bones = mesh->mNumBones;
		cout << "Mesh " << i << " has " << num_vertices << " vertices and " << num_indices << " indices" << endl;
		total_vertices += num_vertices;
		total_indices += num_indices;
		total_bones += num_bones;
		if (mesh->HasBones())
			parse_mesh_bones(mesh);
	}
}

void parse_scene(const aiScene* scene)
{
	parse_meshes(scene);
}

// fin testing

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

int main(void)
{
	FMOD_RESULT result;
	FMOD::System* system = nullptr;

	result = FMOD::System_Create(&system);

	system->init(512, FMOD_INIT_NORMAL, 0);
	FMOD::Sound* sound = nullptr;
	// system->createSound("assets/sounds/chilango.mp3", FMOD_DEFAULT, 0, &sound);

	FMOD::Channel* channel = nullptr;
	result = system->playSound(sound, nullptr, false, &channel);

    /* Initialize the library */
	window = Window();
    window.Initialize();
    ImGuiIO& imgui_io = ImGui::GetIO(); (void)imgui_io;

	/*Also testing*/
	LoadShader("Assets/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
	LoadShader("Assets/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);

	// assimp
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("Assets/Models/aurora.glb", ASSIMP_LOAD_FLAGS);
	parse_scene(scene);

	GLuint	uniformProjection = 0,
			uniformModel = 0,
			uniformView = 0,
			uniformEyePosition = 0,
			uniformSpecularIntensity = 0,
			uniformShininess = 0,
			uniformTextureOffset = 0,
			uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.getBufferWidth() / window.getBufferHeight(), 0.1f, 1000.0f);

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
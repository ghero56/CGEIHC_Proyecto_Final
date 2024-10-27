#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi

#include <iostream>
#include <vector>
#include <string>

#include <fmod.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>

#include <assimp/Importer.hpp>

#include "Window.h"

Window window;

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

    bool demoWindow;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.selfWindow))
    {
		glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		ImGui::Begin("Hello, world!");
		ImGui::Text("This is some useful text.");
		ImGui::Checkbox("Demo Window", &demoWindow);
        ImGui::End();
        if (demoWindow)
            ImGui::ShowDemoWindow();
		

		if (imgui_io.WantCaptureMouse)
		{
            
			// all glfw events should be here
		}

        glDisable(GL_BLEND);

        // glUseProgram(0);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window.selfWindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
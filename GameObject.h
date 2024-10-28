#pragma once

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <vector>
#include <string>
#include <fmod.hpp>
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb/stb_image.h"
#include "Texture.h"
#include "Animation.h"
#include "Animator.h"


using namespace std;

class GameObject {
public:
    GameObject();
    ~GameObject();

    // Funciones para configurar y obtener propiedades del objeto
    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() const { return position; };

    void SetRotation(const glm::vec3& rotation);
    glm::vec3 GetRotation() const;

    void SetScale(const glm::vec3& scale);
    glm::vec3 GetScale() const;

    void SetModelMatrix(const glm::mat4& model);
    glm::mat4 GetModelMatrix() const;

    void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
    void CreateMesh(const std::string& filename);

    // Animaci�n
    void SetBoneTransforms(const std::vector<glm::mat4>& transforms);
    const std::vector<glm::mat4>& GetBoneTransforms() const;

    // Para agregar o eliminar hijos
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);

    // Renderizaci�n y actualizaci�n
    void Update(float deltaTime);
    void Render();

    // Funciones de sonido
    void PlaySound(const string& soundFile);
    void StopSound();

	void LoadMaterials(const aiScene* scene);
	void LoadNode(aiNode* node, const aiScene* scene);
    void LoadMesh(aiMesh* mesh, const aiScene* scene);

private:
    Mesh* mesh;                 // mesh o modelo 3D

    glm::mat4 model;            // matriz de transformaci�n
    glm::vec3 position;         // posici�n en el mundo
    glm::vec3 rotation;         // rotaci�n
    glm::vec3 scale;            // escala

	std::vector<Texture*> textureList;

    bool selected;
    bool showGizmos;
    bool showBoundingBox;
    bool showWireframe;
    bool showNormals;
    bool showBones;
    bool showSkeleton;
    bool showSkeletonJoints;
    bool showSkeletonBones;
    bool showSkeletonNames;
    bool showSkeletonWeights;

    bool showSelfWindow;         // imgui self window para ver sus caracter�sticas al seleccionarlo

    FMOD::System* soundSystem;   // generador de sonido
    FMOD::Sound* sound;          // sonido
    FMOD::Channel* channel;      // canal de sonido

    // el animador y sus animaciones
	Animation* animation;
	Animator* animator;

    GameObject* parent;          // padre (puede ser nullptr)
    vector<GameObject*> children; // hijos (puede estar vac�o el vector)

    std::vector<glm::mat4> boneTransforms; // transformaciones de huesos para animaciones
};

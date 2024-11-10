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
#include "Texture.h"
#include "include/stb/stb_image.h"
#include "Animation.h"
#include "Animator.h"

using namespace std;

class GameObject {
public:
    GameObject();
    GameObject(char*);
    GameObject(char* name, GameObject* parent);

    void Animate(float deltaTime);

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

    // Animación
    void SetBoneTransforms(const std::vector<glm::mat4>& transforms);
    const std::vector<glm::mat4>& GetBoneTransforms() const;

    // Para agregar o eliminar hijos
    void AddChild(GameObject* child);
    void RemoveChild(GameObject* child);

    // Renderización y actualización
    void Update(float deltaTime);
    void Render();

    // Funciones de sonido
    void PlaySound(const string& soundFile);
    void StopSound();

	void LoadMaterials(const aiScene* scene);
	void LoadNode(aiNode* node, const aiScene* scene);
    void LoadMesh(aiMesh* mesh, const aiScene* scene);

	bool IsSelected() const { return selected; }
    void SetSelected(bool selected) { this->selected = selected; }

    void EditorTools(bool hide);

    bool HasAnimation() const { return (animations.size() > 0); }

    int GetNumAnimations() const { return animations.size(); }

	void SetCurrentAnimation(int index) { if (index >= 0 && index < animations.size()) animator->PlayAnimation(animations[index]); }

    void UseLight(GLuint , GLuint , GLuint , GLuint );

    void color4_to_float4(const aiColor4D* c, float f[4]);
	void set_float4(float f[4], float a, float b, float c, float d);
	void apply_material(const aiMaterial* mtl);

	glm::vec3 GetObjectPosition() { return glm::vec3(model[3][0], model[3][1], model[3][2]); }
	glm::vec3 GetObjectFront() { return glm::vec3(model[2][0], model[2][1], model[2][2]); }
	glm::vec3 GetCameraOffset() { return glm::vec3(0.0f, 20.0f, -20.0f); }

    ~GameObject();

private:
    // el animador y sus animaciones
    vector<Animation*> animations;
    Animator* animator;	

	const aiScene* scene;
    aiNode* nodes;
	aiMesh* meshes;
	aiMaterial* materials;

	GLuint* textureIds;
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
    bool bindScale;

    bool showSelfWindow;         // imgui self window para ver sus características al seleccionarlo

    FMOD::System* soundSystem;   // generador de sonido
    FMOD::Sound* sound;          // sonido
    FMOD::Channel* channel;      // canal de sonido

    glm::mat4 model;            // matriz de transformación
    glm::vec3 position;         // posición en el mundo
    glm::vec3 rotation;         // rotación
    glm::vec3 scale;            // escala

    GameObject* parent;          // padre (puede ser nullptr)

    Texture* defaultTexture;

    vector<GameObject*> children; // hijos (puede estar vacío el vector)
    vector<Texture*> textureList;
    vector<Mesh*> mesh;                 // meshes o modelos 3D
    vector<unsigned int> materialFaces;     // índices de los meshes
    vector<glm::mat4> boneTransforms; // transformaciones de huesos para animaciones

	char* name;                // nombre del objeto
};

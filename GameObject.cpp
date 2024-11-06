#include "GameObject.h"
#include <jsoncons/json_encoder.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp> // Incluir las funciones de cuaterniones
#include <algorithm>
#include "include\stb\stb_image.h"
#include "COMMODO_VALUES.h"

GameObject::GameObject(char* name):
	name(name), selected(false), showGizmos(false), showBoundingBox(false),
    showWireframe(false), showNormals(false), showBones(false), showSkeleton(false),
    showSkeletonJoints(false), showSkeletonBones(false), showSkeletonNames(false),
    showSkeletonWeights(false), showSelfWindow(false), soundSystem(nullptr),
    sound(nullptr), channel(nullptr), parent(nullptr), animator(nullptr), light(nullptr) {

	animations.clear();

	mesh.empty();

    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
}

GameObject::GameObject(char* name, GameObject* parent) :
    name(name), selected(false), showGizmos(false), showBoundingBox(false),
    showWireframe(false), showNormals(false), showBones(false), showSkeleton(false),
    showSkeletonJoints(false), showSkeletonBones(false), showSkeletonNames(false),
    showSkeletonWeights(false), showSelfWindow(false), soundSystem(nullptr),
    sound(nullptr), channel(nullptr), parent(parent), animator(nullptr), light(nullptr) {

    animations.clear();

    mesh.empty();

    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
}

GameObject::GameObject() :
    selected(false), showGizmos(false), showBoundingBox(false),
    showWireframe(false), showNormals(false), showBones(false), showSkeleton(false),
    showSkeletonJoints(false), showSkeletonBones(false), showSkeletonNames(false),
    showSkeletonWeights(false), showSelfWindow(false), soundSystem(nullptr),
    sound(nullptr), channel(nullptr), parent(nullptr), animator(nullptr), light(nullptr) {

    animations.clear();

    mesh.empty();

    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
}

GameObject::GameObject(char* name, Light* light) {
	this->name = name;
	this->light = light;
}

GameObject::GameObject(char* name, GameObject* parent, Light* light) {
	this->name = name;
	this->light = light;
	this->parent = parent;
}

void GameObject::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation, GLuint directionLocation) {
    if (!light)
        return;
	light->UseLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation, directionLocation);
}

void GameObject::SetPosition(const glm::vec3& newPosition) {
    if (light) {
        light->SetPosition(newPosition);
		return;
    }
    position = newPosition;
    // Actualiza la matriz de modelo cuando la posici�n cambia
    model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
    // model = glm::translate(model, position);
}

void GameObject::SetRotation(const glm::vec3& newRotation) {
    rotation = newRotation;
    // Actualiza la matriz de modelo cuando la rotaci�n cambia
    model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
}

glm::vec3 GameObject::GetRotation() const {
    return rotation;
}

void GameObject::SetScale(const glm::vec3& newScale) {
    scale = newScale;
    // Actualiza la matriz de modelo cuando la escala cambia
    model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
}

glm::vec3 GameObject::GetScale() const {
    return scale;
}

void GameObject::SetModelMatrix(const glm::mat4& newModel) {
    model = newModel;
}

glm::mat4 GameObject::GetModelMatrix() const {
    return model;
}

void GameObject::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
    Mesh* newMesh = new Mesh();
    newMesh->CreateMesh(vertices, indices, numOfVertices, numOfIndices);
	mesh.push_back(newMesh);
}

void GameObject::CreateMesh(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

    if (!scene) {
		cout << "Failed to load model: " << filename << endl;
        return;
    }

    LoadNode(scene->mRootNode, scene);
    LoadMaterials(scene);

    // vamos a usar las texturas


    if (scene->mAnimations != nullptr) {
        // Cargar animaciones, si existen
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		    animations.push_back(new Animation(scene->mAnimations[i]));
        animator = new Animator(animations);
    }
}

void GameObject::Animate(float deltaTime) {
    if (animator && animations.size() > 0) {
        animator->UpdateAnimation(deltaTime);
        // Actualizamos las transformaciones de huesos con las matrices calculadas por el animador
        boneTransforms = animator->GetFinalBoneMatrices();
    }
}

void GameObject::SetBoneTransforms(const std::vector<glm::mat4>& transforms) {
    boneTransforms = transforms;
}

const std::vector<glm::mat4>& GameObject::GetBoneTransforms() const {
    return boneTransforms;
}

void GameObject::AddChild(GameObject* child) {
    children.push_back(child);
    child->parent = this;
}

void GameObject::RemoveChild(GameObject* child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
    child->parent = nullptr;
}

void GameObject::Update(float deltaTime) {
    // Actualiza la l�gica del objeto aqu�
    for (auto& child : children) {
        child->Update(deltaTime);
    }
}

void GameObject::Render() {
    for (int i = 0; i < mesh.size(); i++) {
		unsigned int materialIndex = materialFaces[i];
        //std::cout << "Renderizando mesh " << i << " con materialIndex " << materialIndex << std::endl;
        if (materialIndex < textureList.size() && textureList[materialIndex]) {
            textureList[materialIndex]->UseTexture();
            //std::cout << "Usando textura " << materialIndex << " con ID: " << textureList[materialIndex]->GetID() << std::endl;
        }
        else {
            //std::cout << "No se encontr� textura v�lida para materialIndex " << materialIndex << std::endl;
        }
        mesh[i]->RenderMesh();
	}
    for (auto& child : children) {
        child->Render();
    }
}

void GameObject::PlaySound(const std::string& soundFile) {
    if (soundSystem) {
        soundSystem->createSound(soundFile.c_str(), FMOD_DEFAULT, 0, &sound);
        soundSystem->playSound(sound, 0, false, &channel);
    }
}

void GameObject::StopSound() {
    if (channel) {
        channel->stop();
    }
}

void GameObject::LoadNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        LoadNode(node->mChildren[i], scene);
    }
}

void GameObject::LoadMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

        if (mesh->mTextureCoords[0]) {
            vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
        }
        else {
            vertices.insert(vertices.end(), { 0.0f, 0.0f });
        }

		// las normales deben invertirse para que se vean correctamente
        vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
        // vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    /*if (this->mesh) {
        delete this->mesh;
    }*/

    Mesh* newMesh = new Mesh();
    newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());

	this->mesh.push_back(newMesh);
	this->materialFaces.push_back(mesh->mMaterialIndex);
}

void GameObject::LoadMaterials(const aiScene* scene) {
    // Crear una textura por defecto est�tica solo una vez
    static Texture* defaultTexture = nullptr;
    if (!defaultTexture) {
        defaultTexture = new Texture("Assets/Textures/plain.png");
        if (!defaultTexture->LoadTexture(true, false)) {
            std::cerr << "Error: No se pudo cargar la textura por defecto." << std::endl;
            delete defaultTexture;
            defaultTexture = nullptr;
        }
    }

    textureList.resize(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        textureList[i] = nullptr;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                const aiTexture* tex = scene->GetEmbeddedTexture(path.C_Str());
                if (tex) {
                    // L�gica para cargar textura embebida...
                }
                else {
                    std::string pathStr = path.C_Str();
                    int idx = pathStr.rfind("\\");
                    std::string filename = pathStr.substr(idx + 1);
                    std::string texPath = "Assets/Textures/" + filename;
                    textureList[i] = new Texture(texPath.c_str());

                    bool hasAlpha = (filename.find("tga") != std::string::npos || filename.find("png") != std::string::npos);
                    if (!textureList[i]->LoadTexture(hasAlpha, false)) {
                        std::cout << "Fall� en cargar la Textura: " << texPath << std::endl;
                        delete textureList[i];
                        textureList[i] = nullptr;
                    }
                    else {
                        std::cout << "Textura cargada: " << texPath << std::endl;
                    }
                }
            }
        }

        // Si la textura no se carg� correctamente, usa la textura por defecto
        if (textureList[i] == nullptr) {
            std::cout << "Se asigna la textura por defecto para material " << i << std::endl;
            textureList[i] = defaultTexture;
        }
    }
}

void GameObject::EditorTools(bool hide){
	if (hide) {
		return;
	}

	float localx = position.x;
	float localy = position.y;
	float localz = position.z;

	ImGui::Begin(this->name);

    /* Debugs
    ImGui::BeginListBox("Meshes");
	for (int i = 0; i < mesh.size(); i++) {
		ImGui::Text("Mesh %d", i);
	}
	ImGui::EndListBox();
    */
	
	ImGui::Text("Position");
	ImGui::SliderFloat("Position X", &localx, -10.0f, 10.0f);
	ImGui::SliderFloat("Position Y", &localy, -10.0f, 10.0f);
	ImGui::SliderFloat("Position Z", &localz, -10.0f, 10.0f);

	SetPosition(glm::vec3(localx, localy, localz));

	ImGui::Text("Rotation");
	ImGui::SliderFloat("Rotation X", &rotation.x, -180.0f, 180.0f);
	ImGui::SliderFloat("Rotation Y", &rotation.y, -180.0f, 180.0f);
	ImGui::SliderFloat("Rotation Z", &rotation.z, -180.0f, 180.0f);

    // la escala NO PUEDE ser 0
	ImGui::Text("Scale");
	ImGui::Checkbox("Bind scale", &bindScale);
	if (bindScale) {
        ImGui::SliderFloat("Scale global", &scale.x, 0.001f, 10.0f);
		scale.x = scale.y = scale.z = scale.x;
    }
    else {
        ImGui::SliderFloat("Scale X", &scale.x, 0.001f, 10.0f);
        ImGui::SliderFloat("Scale Y", &scale.y, 0.001f, 10.0f);
        ImGui::SliderFloat("Scale Z", &scale.z, 0.001f, 10.0f);
    }
	
	ImGui::Checkbox("Show Gizmos", &showGizmos);
	ImGui::Checkbox("Show Bounding Box", &showBoundingBox);
	ImGui::Checkbox("Show Wireframe", &showWireframe);
	ImGui::Checkbox("Show Normals", &showNormals);
	ImGui::Checkbox("Show Bones", &showBones);
	ImGui::Checkbox("Show Skeleton", &showSkeleton);
	ImGui::Checkbox("Show Skeleton Joints", &showSkeletonJoints);
	ImGui::Checkbox("Show Skeleton Bones", &showSkeletonBones);
	ImGui::Checkbox("Show Skeleton Names", &showSkeletonNames);
	ImGui::Checkbox("Show Skeleton Weights", &showSkeletonWeights);

	ImGui::Checkbox("Show Self Window", &showSelfWindow);

	if (ImGui::Button("Add Child")) {
		GameObject* child = new GameObject(name + (char)children.size());
		child->SetPosition(glm::vec3(1.0f, 1.0f, 1.0f));
		AddChild(child);
	}

	if (ImGui::Button("Remove Child")) {
		if (children.size() > 0) {
			GameObject* child = children.back();
			RemoveChild(child);
			delete child;
		}
	}

	ImGui::BeginChildFrame(1, ImVec2(0, 300), ImGuiWindowFlags_HorizontalScrollbar);
	for (auto& child : children) {
		child->EditorTools(false);
	}
	ImGui::EndChildFrame();

	ImGui::End();
}

GameObject::~GameObject() {
    if (sound) {
        sound->release();
    }
    if (soundSystem) {
        soundSystem->release();
    }
    if (animator) {
        delete animator;
    }
    if (light) {
        delete light;
    }
    for (auto& child : children) {
        delete child;
    }
    if (mesh.size() > 0) {
        for (auto& m : mesh) {
            delete m;
        }
    }
    if (textureList.size() > 0) {
        for (auto& t : textureList) {
            delete t;
        }
    }
    for (auto& a : animations) {
        delete a;
    }
    if (boneTransforms.size() > 0) {
        boneTransforms.clear();
    }
    if (parent) {
        parent->RemoveChild(this);
    }
}
#include "GameObject.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/quaternion.hpp> // Incluir las funciones de cuaterniones
#include <algorithm>
#include "include\stb\stb_image.h"

GameObject::GameObject() :
    mesh(nullptr), selected(false), showGizmos(false), showBoundingBox(false),
    showWireframe(false), showNormals(false), showBones(false), showSkeleton(false),
    showSkeletonJoints(false), showSkeletonBones(false), showSkeletonNames(false),
    showSkeletonWeights(false), showSelfWindow(false), soundSystem(nullptr),
    sound(nullptr), channel(nullptr), parent(nullptr) {
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
}

GameObject::~GameObject() {
    if (mesh) {
        delete mesh;
    }
    for (auto child : children) {
        delete child;
    }
    if (sound) {
        sound->release();
    }
    if (soundSystem) {
        soundSystem->close();
        soundSystem->release();
    }
}

void GameObject::SetPosition(const glm::vec3& newPosition) {
    position = newPosition;
    // Actualiza la matriz de modelo cuando la posición cambia
    model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
}

void GameObject::SetRotation(const glm::vec3& newRotation) {
    rotation = newRotation;
    // Actualiza la matriz de modelo cuando la rotación cambia
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
    if (mesh) {
        delete mesh;
    }
    mesh = new Mesh();
    mesh->CreateMesh(vertices, indices, numOfVertices, numOfIndices);
}

void GameObject::CreateMesh(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

    if (!scene) {
        printf("Failed to load model: %s \n", filename.c_str(), importer.GetErrorString());
        return;
    }

    LoadNode(scene->mRootNode, scene);
    LoadMaterials(scene);

    if (scene->mAnimations != nullptr) {
        // Cargar animaciones, si existen
        animation = new Animation(filename, scene);
        animator = new Animator(animation);
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
    // Actualiza la lógica del objeto aquí
    for (auto& child : children) {
        child->Update(deltaTime);
    }
}

void GameObject::Render() {
    if (mesh) {
        mesh->RenderMesh();
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

        vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (this->mesh) {
        delete this->mesh;
    }
    this->mesh = new Mesh();
    this->mesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
}

void GameObject::LoadMaterials(const aiScene* scene) {
    textureList.resize(scene->mNumMaterials);

    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        textureList[i] = nullptr;

        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                const aiTexture* tex = scene->GetEmbeddedTexture(path.C_Str());
                if (tex) {
                    int width, height, channels;
                    unsigned char* imageData = nullptr;
                    if (tex->mHeight == 0) {
                        imageData = stbi_load_from_memory(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, &width, &height, &channels, STBI_rgb_alpha);
                    }
                    else {
                        imageData = reinterpret_cast<unsigned char*>(tex->pcData);
                        width = tex->mWidth;
                        height = tex->mHeight;
                        channels = 4;
                    }

                    if (imageData) {
                        textureList[i] = new Texture(imageData, width, height, channels);
                        if (tex->mHeight == 0) {
                            stbi_image_free(imageData);
                        }
                    }
                }
                else {
                    std::string pathStr = path.C_Str();
                    int idx = pathStr.rfind("\\");
                    std::string filename = pathStr.substr(idx + 1);
                    std::string texPath = "Assets/Textures/" + filename;
                    textureList[i] = new Texture(texPath.c_str());

                    if (!textureList[i]->LoadTexture(filename.find("tga") != std::string::npos || filename.find("png") != std::string::npos)) {
                        printf("Falló en cargar la Textura :%s\n", texPath.c_str());
                        delete textureList[i];
                        textureList[i] = nullptr;
                    }
                }
            }
        }

        if (textureList[i] == nullptr) {
            textureList[i] = new Texture("Textures/plain.png");
            textureList[i]->LoadTexture(true);
        }
    }
}


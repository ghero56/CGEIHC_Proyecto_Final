#include "GameObject.h"
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
    sound(nullptr), channel(nullptr), parent(nullptr), animator(nullptr) {

	animations.clear();

	mesh.empty();
	defaultTexture = nullptr;
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
    sound(nullptr), channel(nullptr), parent(parent), animator(nullptr) {
    defaultTexture = nullptr;
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
    sound(nullptr), channel(nullptr), parent(nullptr), animator(nullptr){
    defaultTexture = nullptr;
    animations.clear();

    mesh.empty();

    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    model = glm::mat4(1.0f);
}

void GameObject::SetPosition(const glm::vec3& newPosition) {
    position = newPosition;
    // Actualiza la matriz de modelo cuando la posición cambia
    model = glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), scale);
    // model = glm::translate(model, position);
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
    Mesh* newMesh = new Mesh();
    newMesh->CreateMesh(vertices, indices, numOfVertices, numOfIndices);
	mesh.push_back(newMesh);
}

void GameObject::CreateMesh(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

    if (!scene) {
		cout << "Failed to load model: " << filename << endl;
		cout << "Error: " << importer.GetErrorString() << endl;
        return;
    }

    this->scene = scene;
	this->nodes = scene->mRootNode;

	glBindVertexArray(0);

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
    // Actualiza la lógica del objeto aquí
    for (auto& child : children) {
        child->Update(deltaTime);
    }
}


void GameObject::Render() {
    for (int i = 0; i < mesh.size(); i++) {
        unsigned int materialIndex = materialFaces[i];

        // Comprobamos si hay una textura válida para el material actual
        if (materialIndex < textureList.size() && textureList[materialIndex]) {
            glActiveTexture(GL_TEXTURE0); // Activamos la textura
            textureList[materialIndex]->UseTexture(); // Vinculamos la textura
        }
        else {
            // Si no hay textura válida, usamos la textura por defecto
            glActiveTexture(GL_TEXTURE0);
            defaultTexture->UseTexture();
        }

        mesh[i]->RenderMesh(); // Renderizamos la malla
    }

    for (auto& child : children) {
        child->Render();
    }
}

/*
void GameObject::Render() {  
    for (int i = 0; i < mesh.size(); i++) {
		unsigned int materialIndex = materialFaces[i];
        //std::cout << "Renderizando mesh " << i << " con materialIndex " << materialIndex << std::endl;
        if (materialIndex < textureList.size() && textureList[materialIndex]) {
            textureList[materialIndex]->UseTexture();
            //std::cout << "Usando textura " << materialIndex << " con ID: " << textureList[materialIndex]->GetID() << std::endl;
        }
        else {
            //std::cout << "No se encontró textura válida para materialIndex " << materialIndex << std::endl;
        }
        mesh[i]->RenderMesh();
	}
    

    for (auto& child : children) {
        child->Render();
    }
}
*/

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

        vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    Mesh* newMesh = new Mesh();
    newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());

    this->mesh.push_back(newMesh);
    this->materialFaces.push_back(mesh->mMaterialIndex);
}


void GameObject::color4_to_float4(const aiColor4D* c, float f[4])
{
    f[0] = c->r;
    f[1] = c->g;
    f[2] = c->b;
    f[3] = c->a;
}

void GameObject::set_float4(float f[4], float a, float b, float c, float d)
{
    f[0] = a;
    f[1] = b;
    f[2] = c;
    f[3] = d;
}

void GameObject::apply_material(const aiMaterial* mtl) {
    float c[4];
    int ret1, ret2;
    aiColor4D diffuse;
    aiColor4D specular;
    aiColor4D ambient;
    aiColor4D emission;
    float shininess, strength;
    unsigned int max;	// changed: to unsigned

    set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
    if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) color4_to_float4(&diffuse, c);

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

    set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
    if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular)) color4_to_float4(&specular, c);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

    set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
    if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient)) color4_to_float4(&ambient, c);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

    set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
    if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission)) color4_to_float4(&emission, c);

    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

    max = 1;
    ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
    max = 1;
    ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
    if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
    {
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
    }
    else
    {
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
        set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GameObject::LoadMaterials(const aiScene* scene) {
    // Crear una textura por defecto estática solo una vez
    if (!defaultTexture) {
        defaultTexture = new Texture("Assets/Textures/plain.png");
        if (!defaultTexture->LoadTexture(true, false, 0,0)) {
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
					textureList[i] = new Texture(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, tex->mHeight, tex->CheckFormat("rgba") ? 4 : 3);
                    if (!textureList[i]->LoadTexture(tex->CheckFormat("rgba"), true, tex->mWidth, tex->mHeight)) {
                            cout << "Failed to load embbed texture: " << endl; 
                            delete textureList[i]; 
                            textureList[i] = nullptr;
                    }
                }
                else {
                    string pathStr = path.C_Str();
                    int idx = pathStr.rfind("\\");
                    string filename = pathStr.substr(idx + 1);
                    string texPath = "Assets/Textures/" + filename;
                    textureList[i] = new Texture(texPath.c_str());

                    bool hasAlpha = (filename.find("tga") != std::string::npos || filename.find("png") != std::string::npos);
                    if (!textureList[i]->LoadTexture(hasAlpha, false,0,0)) {
                        cout << "Falló en cargar la Textura: " << texPath << std::endl;
                        delete textureList[i];
                        textureList[i] = nullptr;
                    }
                    else {
                        cout << "Textura cargada: " << texPath << std::endl;
                    }
                }
            }
        }

        // Si la textura no se cargó correctamente, usa la textura por defecto
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
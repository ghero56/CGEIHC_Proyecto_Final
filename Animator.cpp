#include "Animator.h"
#include <glm/gtc/matrix_transform.hpp>

Animator::Animator(Animation* animation)
    : currentAnimation(animation), currentTime(0.0f), finalBoneMatrices(100, glm::mat4(1.0f)) {
}

void Animator::UpdateAnimation(float deltaTime) {
    currentTime += currentAnimation->GetTicksPerSecond() * deltaTime;
    currentTime = fmod(currentTime, currentAnimation->GetDuration()); // Ciclo de la animaci�n

    if (currentAnimation) {
        const aiScene* scene = currentAnimation->GetAssimpScene();
        if (scene) {
            CalculateBoneTransform(scene->mRootNode, glm::mat4(1.0f));
        }
    }
}

void Animator::PlayAnimation(Animation* animation) {
    currentAnimation = animation;
    currentTime = 0.0f;
}

std::vector<glm::mat4> Animator::GetFinalBoneMatrices() {
    return finalBoneMatrices;
}

void Animator::CalculateBoneTransform(const aiNode* node, glm::mat4 parentTransform) {
    std::string nodeName(node->mName.data);
    const aiAnimation* animation = currentAnimation->GetAssimpAnimation();

    // Encuentra el canal de animaci�n correspondiente al nodo actual
    const aiNodeAnim* nodeAnim = nullptr;
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        if (std::string(animation->mChannels[i]->mNodeName.data) == nodeName) {
            nodeAnim = animation->mChannels[i];
            break;
        }
    }

    // Interpolaci�n de las transformaciones de la animaci�n
    glm::mat4 nodeTransform = glm::mat4(1.0f);
    if (nodeAnim) {
        // Interpolaci�n de la posici�n
        aiVector3D position;
        if (nodeAnim->mNumPositionKeys == 1) {
            position = nodeAnim->mPositionKeys[0].mValue;
        }
        else {
            for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++) {
                if (currentTime < nodeAnim->mPositionKeys[i + 1].mTime) {
                    aiVectorKey key1 = nodeAnim->mPositionKeys[i];
                    aiVectorKey key2 = nodeAnim->mPositionKeys[i + 1];
                    float delta = key2.mTime - key1.mTime;
                    float factor = (currentTime - key1.mTime) / delta;
                    position = key1.mValue + factor * (key2.mValue - key1.mValue);
                    break;
                }
            }
        }
        nodeTransform = glm::translate(nodeTransform, glm::vec3(position.x, position.y, position.z));

        // Interpolaci�n de la rotaci�n
        aiQuaternion rotation;
        if (nodeAnim->mNumRotationKeys == 1) {
            rotation = nodeAnim->mRotationKeys[0].mValue;
        }
        else {
            for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++) {
                if (currentTime < nodeAnim->mRotationKeys[i + 1].mTime) {
                    aiQuatKey key1 = nodeAnim->mRotationKeys[i];
                    aiQuatKey key2 = nodeAnim->mRotationKeys[i + 1];
                    float delta = key2.mTime - key1.mTime;
                    float factor = (currentTime - key1.mTime) / delta;
                    aiQuaternion::Interpolate(rotation, key1.mValue, key2.mValue, factor);
                    rotation.Normalize();
                    break;
                }
            }
        }
        nodeTransform *= glm::mat4_cast(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));

        // Interpolaci�n de la escala
        aiVector3D scale;
        if (nodeAnim->mNumScalingKeys == 1) {
            scale = nodeAnim->mScalingKeys[0].mValue;
        }
        else {
            for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++) {
                if (currentTime < nodeAnim->mScalingKeys[i + 1].mTime) {
                    aiVectorKey key1 = nodeAnim->mScalingKeys[i];
                    aiVectorKey key2 = nodeAnim->mScalingKeys[i + 1];
                    float delta = key2.mTime - key1.mTime;
                    float factor = (currentTime - key1.mTime) / delta;
                    scale = key1.mValue + factor * (key2.mValue - key1.mValue);
                    break;
                }
            }
        }
        nodeTransform = glm::scale(nodeTransform, glm::vec3(scale.x, scale.y, scale.z));
    }

    glm::mat4 globalTransform = parentTransform * nodeTransform;

    // Almacenar la transformaci�n final del hueso si es un hueso conocido
    if (boneMapping.find(nodeName) != boneMapping.end()) {
        unsigned int boneIndex = boneMapping[nodeName];
        finalBoneMatrices[boneIndex] = globalTransform * boneOffsets[boneIndex];
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        CalculateBoneTransform(node->mChildren[i], globalTransform);
    }
}


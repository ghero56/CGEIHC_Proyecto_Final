#include "Animation.h"

Animation::Animation(const std::string& filePath, const aiScene* scene) {
    if (scene->mNumAnimations > 0) {
        assimpAnimation = scene->mAnimations[0];
        name = assimpAnimation->mName.C_Str();
        duration = assimpAnimation->mDuration;
        ticksPerSecond = assimpAnimation->mTicksPerSecond != 0 ? assimpAnimation->mTicksPerSecond : 25.0f;
        assimpScene = scene;  // Guardar la escena completa
    }
}

Animation::~Animation() {}

const aiAnimation* Animation::GetAssimpAnimation() const {
    return assimpAnimation;
}

const aiScene* Animation::GetAssimpScene() const {
    return assimpScene;
}

const std::string& Animation::GetName() const {
    return name;
}

float Animation::GetDuration() const {
    return duration;
}

float Animation::GetTicksPerSecond() const {
    return ticksPerSecond;
}

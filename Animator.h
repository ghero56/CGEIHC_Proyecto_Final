#pragma once

#include "Animation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // Para glm::translate, glm::scale
#include <glm/gtc/quaternion.hpp>  // Para glm::quat y glm::mat4_cast
#include <vector>
#include <unordered_map>
#include <string>

class Animator {
public:
    Animator(Animation* animation);
	Animator(std::vector<Animation*> animation);
    void UpdateAnimation(float deltaTime);
    void PlayAnimation(Animation* animation);
    std::vector<glm::mat4> GetFinalBoneMatrices();

private:
    void CalculateBoneTransform(const aiNode* node, glm::mat4 parentTransform);

    std::vector<Animation*> animations;
	Animation* currentAnimation;

    int totalAnimations;

    float currentTime;
    std::vector<glm::mat4> finalBoneMatrices;

    std::unordered_map<std::string, unsigned int> boneMapping;
    std::vector<glm::mat4> boneOffsets;
};

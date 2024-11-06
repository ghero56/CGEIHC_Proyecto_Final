#pragma once
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Animation {
public:
    Animation(const std::string& filePath, const aiScene* scene);
	Animation(const aiAnimation* animation);
    ~Animation();

    const aiAnimation* GetAssimpAnimation() const;
    const aiScene* GetAssimpScene() const;
    const std::string& GetName() const;
    float GetDuration() const;
    float GetTicksPerSecond() const;

private:
    std::string name;
    float duration;
    float ticksPerSecond;
    const aiAnimation* assimpAnimation;
    const aiScene* assimpScene;
};

#pragma once

#include <glew.h>
#include <string>

class Texture {
public:
    Texture(const std::string& filePath);
    Texture(unsigned char* imageData, int width, int height, int channels);
    ~Texture();

    bool LoadTexture(bool alpha);
    void UseTexture();
    void ClearTexture();

private:
    GLuint textureID;
    int width, height, bitDepth;
    std::string filePath;
};

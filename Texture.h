#pragma once

#include <glew.h>
#include <string>
#include <iostream>
#include "include\stb\stb_image.h"

class Texture {
public:
	Texture();
    Texture(const std::string& filePath);
    Texture(unsigned char* imageData, int width, int height, int channels);
    ~Texture();

    bool LoadTexture(bool , bool);
    void UseTexture();
    void ClearTexture();

    GLuint GetID() { return textureID; }

private:
    GLuint textureID;
    int width, height, bitDepth;
    std::string filePath;
	unsigned char* imageData;
};

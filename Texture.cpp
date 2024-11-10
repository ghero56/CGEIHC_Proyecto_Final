#include "Texture.h"
#include "include\stb\stb_image.h"
#include <iostream>

using namespace std;

Texture::Texture()
{
	imageData = nullptr;
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    filePath = "";
}

Texture::Texture(const std::string& filePath) {
	imageData = nullptr;
    this->filePath = filePath;
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
}

Texture::Texture(unsigned char* imageData, int width, int height, int channels) {
	this->imageData = imageData;
    this->filePath = "";
    this->textureID = 0;
    this->width = width;
    this->height = height;
    this->bitDepth = channels;
}

bool Texture::LoadTexture(bool alpha, bool embedded, int imageWidth, int imageHeight) {
	cout << "\nLoading texture: " << filePath << endl;
	cout << "Alpha: " << alpha << endl;
	cout << "Embedded: " << embedded << endl;

    //stbi_set_flip_vertically_on_load(true);
	if (embedded) {
		width = imageWidth;
		height = imageHeight;
		if (!imageData) {
			std::cout << "Failed to find textures: " << filePath << std::endl;
			return false;
		}
    }
    else {
        imageData = stbi_load(filePath.c_str(), &width, &height, &bitDepth, alpha ? STBI_rgb_alpha : 0);
        if (!imageData) {
            std::cout << "Failed to find: " << filePath << std::endl;
            return false;
        }
    }

    // let's check if our imageData is not corrupted or null
	if (imageData == NULL) {
		std::cout << "Failed to load texture" << std::endl;
		return false;
	}
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    //stbi_image_free(imageData);
    glBindTexture(GL_TEXTURE_2D, 0);

	UseTexture();

    return true;
}

void Texture::UseTexture() {
	//cout << "Texture ID: " << textureID << endl;
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture()
{
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
    filePath = "";
}

Texture::~Texture()
{
    ClearTexture();
}

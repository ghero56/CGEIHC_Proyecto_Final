#include "Texture.h"
#include "include\stb\stb_image.h"

Texture::Texture(const std::string& filePath) {
    this->filePath = filePath;
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
}

Texture::Texture(unsigned char* imageData, int width, int height, int channels) {
    this->filePath = "";
    this->textureID = 0;
    this->width = width;
    this->height = height;
    this->bitDepth = channels;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    ClearTexture();
}

bool Texture::LoadTexture(bool alpha) {
    filePath = "Assets/"+filePath;
    unsigned char* texData = stbi_load(filePath.c_str(), &width, &height, &bitDepth, alpha ? STBI_rgb_alpha : STBI_rgb);
    if (!texData) {
        std::cout << "Failed to find: " << filePath << std::endl;
        return false;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, alpha ? GL_RGBA : GL_RGB, width, height, 0, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texData);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

void Texture::UseTexture() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::ClearTexture() {
    glDeleteTextures(1, &textureID);
    textureID = 0;
    width = 0;
    height = 0;
    bitDepth = 0;
}

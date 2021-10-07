#pragma once

#include <map>

struct TextureDesc
{
    uint myWidth{};
    uint myHeight{};
    GLenum myFormat{};
    GLenum myInternalFormat{};
    GLenum myFilter = GL_NEAREST;
    GLenum myClamp = GL_CLAMP_TO_EDGE;
    GLenum myAttachment{};
    GLenum myType{};
    std::vector<uint8> myPixels;
};

struct Texture
{
    static SPtr<Texture> Create(const TextureDesc& desc);
    static SPtr<Texture> Create(const std::string& filelocation, GLenum filter = GL_NEAREST, GLenum clamp = GL_CLAMP_TO_BORDER);
    static SPtr<Texture> Create(uint width, uint height, std::vector<uint8> data,
        GLenum format = GL_RGB, GLenum internalFormat = GL_RGB, GLenum filter = GL_NEAREST, GLenum clamp = GL_CLAMP_TO_EDGE, GLenum type = GL_UNSIGNED_INT, GLenum attachment = 0);

    static std::map<std::string, SPtr<Texture>> ourTextures;

    Texture(const TextureDesc& desc);
    ~Texture();

    void Bind(int unit = -1) const;
    void BindAsFramebufferTexture() const;
    void Unbind() const;

    int myWidth = 0;
    int myHeight = 0;
    GLuint myTextureID = 0;
    GLenum myAttachment = 0;
};
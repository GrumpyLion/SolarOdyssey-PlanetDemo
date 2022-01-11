#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Texture.h"

std::map<std::string, SPtr<Texture>> Texture::ourTextures;

SPtr<Texture> Texture::Create(const TextureDesc& desc)
{
    return MakeShared<Texture>(desc);
}

SPtr<Texture> Texture::Create(const std::string& filelocation, GLenum filter /*= GL_NEAREST*/, GLenum clamp /*= GL_CLAMP_TO_BORDER*/)
{
    //if (ourTextures.find(filelocation) != ourTextures.end()) // TODO caching should also check for clamping, filtering ... not only name
    //    return ourTextures[filelocation];

    TextureDesc desc{};

    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* image = stbi_load(filelocation.c_str(),
        &width,
        &height,
        &channels,
        STBI_rgb_alpha);

    GE_ASSERT(image != nullptr);

    desc.myPixels.resize(width * height * channels);
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myFormat = channels == 3 ? GL_RGB : GL_RGBA;
    desc.myInternalFormat = channels == 3 ? GL_RGB16F : GL_RGBA32F;
    desc.myType = GL_UNSIGNED_BYTE;
    desc.myFilter = filter;
    desc.myClamp = clamp;

    memcpy(desc.myPixels.data(), &image[0], width * height * channels);

    stbi_image_free(image);

    auto textureRef = MakeShared<Texture>(desc);
    ourTextures[filelocation] = textureRef; // TODO textures are currently never unloaded since they're sitting in cache
    return textureRef;
}

SPtr<Texture> Texture::Create(uint width, uint height, std::vector<uint8> data, GLenum format, GLenum internalFormat, GLenum filter, GLenum clamp, GLenum type, GLenum attachment)
{
    TextureDesc desc{};
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myPixels = data;
    desc.myFormat = format;
    desc.myInternalFormat = internalFormat;
    desc.myFilter = filter;
    desc.myClamp = clamp;
    desc.myType = type;
    desc.myAttachment = attachment;
    return MakeShared<Texture>(desc);
}

void Texture::Track(SPtr<Texture> texture, const char* name)
{
    ourTextures[name] = texture;
}

//
//////////////////////////////////////////////////////////////////////////
//

Texture::Texture(const TextureDesc& desc)
    : myWidth(desc.myWidth)
    , myHeight(desc.myHeight)
    , myAttachment(desc.myAttachment)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &myTextureID);

    glTextureParameteri(myTextureID, GL_TEXTURE_MIN_FILTER, desc.myFilter);
    glTextureParameteri(myTextureID, GL_TEXTURE_MAG_FILTER, desc.myFilter);
    glTextureParameteri(myTextureID, GL_TEXTURE_WRAP_S, desc.myClamp);
    glTextureParameteri(myTextureID, GL_TEXTURE_WRAP_T, desc.myClamp);

    glTextureParameterfv(myTextureID, GL_TEXTURE_BORDER_COLOR, (float*)&desc.myBorderColor);


    int width = desc.myWidth;
    int height = desc.myHeight;

    glTextureStorage2D(myTextureID, 1, desc.myInternalFormat, width, height);
    
    if (desc.myPixels.size() > 0)
        glTextureSubImage2D(myTextureID, 0, 0, 0, width, height, desc.myFormat, desc.myType, desc.myPixels.data());
    
    if (desc.myAttachment != 0)
        glFramebufferTexture(GL_FRAMEBUFFER, desc.myAttachment, myTextureID, 0);
    else
        glGenerateTextureMipmap(myTextureID);
}

Texture::~Texture()
{
    glDeleteTextures(1, &myTextureID);
}

void Texture::Bind(int unit /*= -1*/) const
{
    glBindTextureUnit(unit, myTextureID);
}

void Texture::BindAsFramebufferTexture(GLuint framebuffer) const
{
    glNamedFramebufferTexture(framebuffer, myAttachment, myTextureID, 0);
}
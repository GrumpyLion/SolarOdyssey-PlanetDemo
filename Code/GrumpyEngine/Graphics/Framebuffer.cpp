#include "pch.h"

#include "Texture.h"
#include "Framebuffer.h"

SPtr<Framebuffer> Framebuffer::Create(const FramebufferDesc& desc)
{
    return MakeShared<Framebuffer>(desc);
}

SPtr<Framebuffer> Framebuffer::Create(uint width, uint height, std::string name /*= ""*/)
{
    FramebufferDesc desc{};
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myName = name;
    return MakeShared<Framebuffer>(desc);
}

Framebuffer::Framebuffer(const FramebufferDesc& desc)
{
    myWidth = desc.myWidth;
    myHeight = desc.myHeight;

    glCreateFramebuffers(1, &myFBO);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &myFBO);
}

void Framebuffer::Finalize()
{
    GE_ASSERT(glCheckNamedFramebufferStatus(myFBO, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void Framebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, myFBO);
    glViewport(0, 0, myWidth, myHeight);
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::AddAttachment(const char* name, SPtr<Texture> texture)
{
    myTextures[name] = texture;
    texture->BindAsFramebufferTexture(myFBO);
}

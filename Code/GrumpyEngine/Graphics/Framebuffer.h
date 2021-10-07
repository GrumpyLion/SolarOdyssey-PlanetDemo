#pragma once

struct Texture;

struct FramebufferDesc
{
    uint myWidth{};
    uint myHeight{};
    std::string myName{};
};

struct Framebuffer
{
    static SPtr<Framebuffer> Create(const FramebufferDesc& desc);
    static SPtr<Framebuffer> Create(uint width, uint height, std::string name = "");

    Framebuffer(const FramebufferDesc& desc);
    ~Framebuffer();

    void Bind();
    void Unbind();
    void AddAttachment(const char* name, SPtr<Texture> texture);

    std::unordered_map<const char*, SPtr<Texture>> myTextures;

    uint myWidth = 0;
    uint myHeight = 0;
    GLuint myFBO = 0;
};
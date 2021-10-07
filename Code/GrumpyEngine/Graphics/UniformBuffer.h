#pragma once

struct UniformBuffer
{
public:
    static SPtr<UniformBuffer> Create(uint bufferSize, uint index, void* data = nullptr, GLenum usage = GL_DYNAMIC_DRAW);

    UniformBuffer(uint bufferSize, uint index, void* data = nullptr, GLenum usage = GL_DYNAMIC_DRAW);
    ~UniformBuffer();

    void Bind();
    void Unbind();
    void SetData(const void* data);

    GLuint myUBO = 0;
    uint mySize = 0;
    uint myIndex = 0;
};
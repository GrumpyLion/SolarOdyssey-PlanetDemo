#include "pch.h"
#include "UniformBuffer.h"

SPtr<UniformBuffer> UniformBuffer::Create(uint bufferSize, uint index, void* data /*= nullptr*/, GLenum usage /*= GL_DYNAMIC_DRAW*/)
{
    return MakeShared<UniformBuffer>(bufferSize, index, data, usage);
}

UniformBuffer::UniformBuffer(uint bufferSize, uint index, void* data /*= nullptr*/, GLenum usage /*= GL_DYNAMIC_DRAW*/)
{
    mySize = bufferSize;
    myIndex = index;

    glGenBuffers(1, &myUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, myUBO);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, data, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, index, myUBO);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &myUBO);
}

void UniformBuffer::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, myUBO);
}

void UniformBuffer::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SetData(const void* data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, myUBO);

    GLvoid* p = glMapBufferRange(GL_UNIFORM_BUFFER, 0, mySize, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT);
    memcpy(p, data, mySize);

    glUnmapBuffer(GL_UNIFORM_BUFFER);
}
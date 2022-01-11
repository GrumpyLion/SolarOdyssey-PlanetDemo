#include "pch.h"
#include "UniformBuffer.h"

SPtr<UniformBuffer> UniformBuffer::Create(uint bufferSize, uint index, const char* debugname /*= "UniformBuffer"*/, void* data /*= nullptr*/, GLenum usage /*= GL_DYNAMIC_DRAW*/)
{
    SPtr<UniformBuffer> buffer = MakeShared<UniformBuffer>();

    buffer->mySize = bufferSize;
    buffer->myIndex = index;

    glGenBuffers(1, &buffer->myUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer->myUBO);
    glBufferData(GL_UNIFORM_BUFFER, bufferSize, data, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glObjectLabel(GL_BUFFER, buffer->myUBO, strlen(debugname), debugname);

    glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer->myUBO);

    return buffer;
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
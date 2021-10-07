#include "pch.h"

#include "Primitives.h"

namespace Primitives
{
    GLuint locQuadVAO = 0;
    GLuint locQuadVBO = 0;
}

void Primitives::Initialize()
{

}

void Primitives::Shutdown()
{
    glDeleteBuffers(1, &locQuadVBO);
    glDeleteVertexArrays(1, &locQuadVAO);
}

void Primitives::DrawFullscreenQuad()
{
    if (locQuadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &locQuadVAO);
        glGenBuffers(1, &locQuadVBO);
        glBindVertexArray(locQuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, locQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(locQuadVAO);

    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glEnable(GL_CULL_FACE);

    glBindVertexArray(0);
}


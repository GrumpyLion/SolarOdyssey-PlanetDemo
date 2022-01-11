#include "pch.h"

#if GE_PLATFORM_PC
#include "Graphics/RendererAPI.h"
#include "Window.h"

namespace RendererAPI
{
    RenderAPICapabilities ourAPICapabilities{};
}

#if !GE_FINAL
void DebugOutput(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    void* userParam)
{
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        GE_PRINT("[OpenGL Debug HIGH] %s", message);
        //GE_ASSERT(false, "GL_DEBUG_SEVERITY_HIGH");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        GE_PRINT("[OpenGL Debug MEDIUM] %s", message);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        GE_PRINT("[OpenGL Debug LOW] %s", message);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        //GE_TRACE("[OpenGL Debug NOTIFICATION] {0}", message);
        break;
    }
}
#endif

void RendererAPI::Initialize()
{
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    GE_ASSERT(status, "GLAD loading error");

#if !GE_FINAL
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)DebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
#endif

    ourAPICapabilities.myVendor = (const char*)glGetString(GL_VENDOR);
    ourAPICapabilities.myRenderer = (const char*)glGetString(GL_RENDERER);
    ourAPICapabilities.myVersion = (const char*)glGetString(GL_VERSION);

    GE_PRINT("OpenGL %s %s %s", ourAPICapabilities.myVendor.c_str(), ourAPICapabilities.myRenderer.c_str(), ourAPICapabilities.myVersion.c_str());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_CLAMP);
    SetLineThickness(2.5f);
}

void RendererAPI::Shutdown()
{

}

void RendererAPI::Clear()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void RendererAPI::SetClearColor(Vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void RendererAPI::SetWireframeMode(bool enable)
{
    if (enable)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RendererAPI::SetLineThickness(float thickness)
{
    glLineWidth(thickness);
}

void RendererAPI::Swap()
{
    glfwSwapBuffers(Window::ourWindowHandle);
}
#endif // GE_PLATFORM_PC
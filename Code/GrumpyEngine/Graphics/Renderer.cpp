#include "pch.h"

#include "Window.h"

#include "Primitives.h"
#include "Lighting.h"
#include "MaterialLibrary.h"
#include "CameraMan.h"
#include "ImGUIWrapper.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Framebuffer.h"
#include "Editor/TopMenuBar.h"
#include "Common/Settings.h"

#if !GE_FINAL
#include "Editor/Editor.h"
#endif

namespace Renderer
{
    Vec2 ourRenderTargetDimensions{};
    FrameData ourFrameData;

    SPtr<Framebuffer> locDepthFrame = nullptr;
    SPtr<Framebuffer> locColorFrame = nullptr;
    SPtr<Material> locCompositeMaterial = nullptr;
    SPtr<CustomRenderer> ourCustomRenderer = nullptr;

#if !GE_FINAL
    bool locRenderDebugView = false;
#endif
}

void Renderer::Initialize()
{
    RendererAPI::Initialize();
    MaterialLibrary::LoadFromJSON("Data/GrumpyEngine/Materials.json"); // load engine internal materials
    Lighting::Initialize();
    CameraMan::Initialize();

#if !GE_FINAL
    ImGuiWrapper::Initialize();

    locRenderDebugView = Settings::GetValue("CustomRendererSettings", false);
    TopMenuBar::ourMenus["Editor"].myItems["Show Custom Renderer Settings"].myOnKlicked.Subscribe(Renderer::Initialize, [&](bool active) { locRenderDebugView = active; });
    TopMenuBar::ourMenus["Editor"].myItems["Show Custom Renderer Settings"].myActive = locRenderDebugView;
#endif

    Window::ourResizeEvent.Subscribe(Renderer::Initialize, [&](int width, int height)
    {
        Resize(width, height);
    });
}

void Renderer::Shutdown()
{
#if !GE_FINAL
    Settings::SetValue("CustomRendererSettings", locRenderDebugView);
#endif

    Window::ourResizeEvent.Unsubscribe(Renderer::Initialize);

#if !GE_FINAL
    ImGuiWrapper::Destroy();
#endif

    Primitives::Shutdown();
    RendererAPI::Shutdown();
}

void Renderer::PreRender()
{
    RendererAPI::Clear();
#if !GE_FINAL
    ImGuiWrapper::PreRender();
#endif
}

void Renderer::Render(float delta)
{
    CameraMan::Render(delta);
    Lighting::Render();

    if (ourCustomRenderer != nullptr)
    {
        ourCustomRenderer->Render(delta);

#if !GE_FINAL
        if (locRenderDebugView)
            ourCustomRenderer->RenderDebug();
#endif

        ourFrameData.Clear();
        CameraMan::PostRender();
        return;
    }

    //////////////////////////////////////////////////////////////////////////
    // Depth
    locDepthFrame->Bind();
    glDepthMask(true);
    glDepthFunc(GL_LESS);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (const FrameData::Instance& instance : ourFrameData.myInstances)
    {
        const Mesh* mesh = instance.myMesh.get();
        const Material* material = instance.myMaterial.get();
        const Mat4& transform = instance.myMatrix;

        material->Bind(RenderPass::Depth);
        material->myShaders[RenderPass::Depth]->BindMatrix4x4("uModelMatrix", transform);
        mesh->Render();
    }

    glDepthFunc(GL_EQUAL);
    glDepthMask(false);
    locDepthFrame->Unbind();
    //
    //////////////////////////////////////////////////////////////////////////
    // Color
    locColorFrame->Bind();
    glClear(GL_COLOR_BUFFER_BIT); 
    for (FrameData::Instance instance : ourFrameData.myInstances)
    {
        const Mesh* mesh = instance.myMesh.get();
        const Material* material = instance.myMaterial.get();
        const Mat4& transform = instance.myMatrix;

        material->Bind(RenderPass::Geometry);
        material->myShaders[RenderPass::Geometry]->BindMatrix4x4("uModelMatrix", transform);
        mesh->Render();
    }

    locColorFrame->Unbind();
    //
    //////////////////////////////////////////////////////////////////////////
    // Composite
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (int)Renderer::ourRenderTargetDimensions.x, (int)Renderer::ourRenderTargetDimensions.y);

    locCompositeMaterial->Bind();
    Primitives::DrawFullscreenQuad();

    ourFrameData.Clear();
    CameraMan::PostRender();
}

void Renderer::PostRender()
{
#if !GE_FINAL
    ImGuiWrapper::Render();
#endif
    RendererAPI::Swap();
}

void Renderer::Resize(int width, int height)
{
    ourRenderTargetDimensions = Vec2(width, height);

    if (ourCustomRenderer != nullptr)
    {
        ourCustomRenderer->Resize(width, height);
        return;
    }

    TextureDesc desc{};
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myFilter = GL_LINEAR;

    locColorFrame = Framebuffer::Create(width, height);
    desc.myAttachment = GL_COLOR_ATTACHMENT0;
    desc.myInternalFormat = GL_RGBA32F;
    desc.myFormat = GL_RGBA;
    desc.myType = GL_UNSIGNED_BYTE;
    desc.myClamp = GL_MIRRORED_REPEAT;
    locColorFrame->AddAttachment("Color", Texture::Create(desc));
    desc.myClamp = GL_CLAMP_TO_BORDER;

    //////////////////////////////////////////////////////////////////////////

    locDepthFrame = Framebuffer::Create(width, height);
    desc.myAttachment = GL_DEPTH_ATTACHMENT;
    desc.myInternalFormat = GL_DEPTH24_STENCIL8;
    desc.myFormat = GL_DEPTH_STENCIL;
    desc.myType = GL_UNSIGNED_INT_24_8;
    locDepthFrame->AddAttachment("Depth", Texture::Create(desc));
    locColorFrame->AddAttachment("Depth", locDepthFrame->myTextures["Depth"]);

    locCompositeMaterial = Material::Create("Composite");
    locCompositeMaterial->AddTexture("uColor", locColorFrame->myTextures["Color"]);
}

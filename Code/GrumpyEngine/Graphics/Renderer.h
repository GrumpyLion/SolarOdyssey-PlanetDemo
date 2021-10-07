#pragma once

#include "RendererAPI.h"
#include "FrameData.h"

struct CustomRenderer
{
    virtual ~CustomRenderer() = default;
    virtual void Render(float delta) = 0;

    virtual void Resize(int width, int height) {};

#if !GE_FINAL
    virtual SPtr<Texture> GetFrameTexture() { return nullptr; }
    virtual void RenderDebug() {}
#endif
};

namespace Renderer
{
    void Initialize();
    void Shutdown();

    void PreRender();
    void Render(float delta);
    void PostRender();

    void Resize(int width, int height);

    //////////////////////////////////////////////////////////////////////////

    extern Vec2 ourRenderTargetDimensions;
    extern FrameData ourFrameData;
    extern SPtr<CustomRenderer> ourCustomRenderer;
};
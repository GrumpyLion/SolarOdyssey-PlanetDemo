#pragma once

namespace RendererAPI
{
    struct RenderAPICapabilities
    {
        std::string myRenderer;
        std::string myVendor;
        std::string myVersion;

        int myMaxSamples = 0;
        float myMaxAnisotropy = 0.0f;
        int myMaxTextureUnits = 0;
    };

    void Initialize();
    void Shutdown();

    void Clear();
    void SetClearColor(Vec4 color);
    void SetWireframeMode(bool enable);
    void SetLineThickness(float thickness);
    void Swap();

    //////////////////////////////////////////////////////////////////////////

    extern RenderAPICapabilities ourAPICapabilities;
}
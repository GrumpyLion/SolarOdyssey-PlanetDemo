#pragma once

#include "Graphics/Renderer.h"

struct Framebuffer;
struct UniformBuffer;

struct SolarOdysseyRenderer final : public CustomRenderer
{
    SolarOdysseyRenderer();
    ~SolarOdysseyRenderer();

    void Render(float delta) override;
    void Resize(int width, int height);

#if !GE_FINAL
    SPtr<Texture> GetFrameTexture();
    void RenderDebug() override;
#endif

    void CalculateCoefficients();
   
    struct AtmosphereBuffer
    {
        float myDensityFalloff = 2.6f;
        float myNumInScatterPoints = 10.0f;
        float myNumOpticalDepthPoints = 100.0f;
        float myPlanetRadius = 200.0f;

        float myAtmosphereHeight = 40.0f;
        float myAtmosphereRadius = myPlanetRadius + myAtmosphereHeight;
        Vec2 padding;

        Vec3 myPlanetCenter = Vec3(0.0f, 0.0f, 0.0f);
        float myScatteringStrength = 1.0f;

        Vec3 myScatterCoeffecients;
        float myIntensity = 10.0f;

        float myDitherStrength = 1.0f;
        float myDitherScale = 1.5f;
    };
    AtmosphereBuffer myAtmosphereBuffer;
    SPtr<UniformBuffer> myAtmosphereUniformBuffer;

    struct WaterBuffer
    {
        Vec4 mySpecularColor = Vec4(0.75f);
        Vec4 myWaterColor1 = Vec4(0.24f, 0.54f, 0.68f, 1.0f);
        Vec4 myWaterColor2 = Vec4(0.05f, 0.15f, 0.35f, 1.0f);
        float myDepthMultiplier = 2.0f;
        float myAlphaMultiplier = 15.0f;
        float mySmoothness = 0.75f;
        float myWaveSpeed = 0.05f;
        float myWaveNormalScale = 0.2f;
        float myWaveStrength = 0.7f;
    };
    WaterBuffer myWaterBuffer;
    SPtr<UniformBuffer> myWaterUniformBuffer;

    Vec3 myWaveLengths = Vec3(700, 530, 460);

    SPtr<Framebuffer> myDepthFrame;
    SPtr<Framebuffer> myColorFrame;
    SPtr<Framebuffer> myCloudFrame;
    SPtr<Framebuffer> myAtmosphereTexturePrecomputeFrame;
#if !GE_FINAL
    SPtr<Framebuffer> myFinalFrame;
    SPtr<Texture> myFinalFrameTexture;
#endif

    SPtr<Mesh> mySunMesh;
    SPtr<Material> mySunMaterial;
    SPtr<Material> myAtmosphereTexturePrecomputeMaterial;
    SPtr<Material> myFinalMaterial;
    bool myWireframeEnabled = false;
};
#pragma once

#include "Framebuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "Material.h"
//
//struct SpotLight : public RefCounted
//{
//    static Ref<SpotLight> Create(int width, int height);
//
//    SpotLight(int width, int height);
//    void RenderDepth();
//
//    struct Buffer
//    {
//        Mat4 myProjectionMatrix;
//        Mat4 myViewMatrix;
//        Vec3 myLightPos;
//        float myFarPlane;
//    };
//    Ref<UniformBuffer> myBuffer{};
//    Ref<Framebuffer> myFramebuffer{};
//    Ref<Texture> myDepthTexture{};
//    Ref<Material> myDepthMaterial{};
//
//    Vec3 myRotation{};
//    Vec3 myPosition{};
//    float mySpotAngle = 0.0f;
//    float myFarPlane = 10.0f;
//    int myWidth = 256;
//    int myHeight = 256;
//};
//
////////////////////////////////////////////////////////////////////////////
//
//struct DirectionalLight : public RefCounted
//{
//    static Ref<DirectionalLight> Create(int width, int height);
//
//    DirectionalLight(int width, int height);
//    void RenderDepth();
//
//    struct Buffer
//    {
//        Mat4 myProjectionMatrix;
//        Mat4 myViewMatrix;
//    };
//    Ref<UniformBuffer> myBuffer{};
//    Ref<Framebuffer> myFramebuffer{};
//    Ref<Texture> myDepthTexture{};
//    Ref<Material> myDepthMaterial{};
//
//    Vec3 myRotation{};
//    Vec3 myPosition{};
//    float myArea = 100.0f;
//    float myFar = 100.0f;
//    int myWidth = 256;
//    int myHeight = 256;
//};

//////////////////////////////////////////////////////////////////////////

namespace Lighting
{
    void Initialize();
    void Render();

    struct DirectionalLightData
    {
        Vec4 myColor{};
        Vec3 myDirection{};
        float myPadding;
    };

    struct SpotLightData
    {
        Vec4 myColor;
        Vec3 myPosition;
        float myPadding;
        Vec3 myDirection;
        float myCutOff;
        float myOuterCutOff;
        float myDistance;
        float myIntensity;
    };

    void SetDirectionalLight(const DirectionalLightData& light, const Vec3& rotation);
    void AddSpotLight(const SpotLightData& light, const Vec3& rotation, bool shadows, float spotAngle);

    constexpr int ourMaxPointLightCount = 16;
    constexpr int ourMaxSpotLightCount = 16;
    struct LightBuffer
    {
        DirectionalLightData myDirectionalLight;
        Vec3 myAmbientColor = Vec3(0.15f, 0.15f, 0.15f);
        int mySpotLightCount = 0;
        SpotLightData mySpotLights[ourMaxSpotLightCount];
    };

    //extern Ref<SpotLight> ourSpotLight;
    //extern Ref<DirectionalLight> ourDirectionalLight;

    extern LightBuffer ourLightBuffer;
    extern SPtr<UniformBuffer> ourLightUniformBuffer;
}
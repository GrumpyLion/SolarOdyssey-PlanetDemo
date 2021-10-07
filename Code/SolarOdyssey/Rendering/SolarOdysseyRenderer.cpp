#include "pch.h"

#include "Common/Window.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Primitives.h"
#include "Graphics/RendererGlobals.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/CameraMan.h"
#include "Graphics/Lighting.h"
#include "SolarOdysseyRenderer.h"

#if !GE_FINAL
#include <imgui.h>
#include "Editor/Editor.h"
#endif

SolarOdysseyRenderer::SolarOdysseyRenderer()
{
    CalculateCoefficients();
    myAtmosphereUniformBuffer = UniformBuffer::Create(sizeof(AtmosphereBuffer), (uint)UniformBufferIndexes::AtmosphereBuffer, &myAtmosphereBuffer, GL_STATIC_DRAW);
    myWaterUniformBuffer = UniformBuffer::Create(sizeof(WaterBuffer), (uint)UniformBufferIndexes::WaterBuffer, &myWaterBuffer, GL_STATIC_DRAW);

    //////////////////////////////////////////////////////////////////////////
    // precompute optical depth for atmosphere
    TextureDesc precomputeDesc{};
    precomputeDesc.myWidth = 256;
    precomputeDesc.myHeight = 256;
    precomputeDesc.myFilter = GL_LINEAR;
    precomputeDesc.myAttachment = GL_COLOR_ATTACHMENT0;
    precomputeDesc.myInternalFormat = GL_RGB16F;
    precomputeDesc.myFormat = GL_RGB;
    precomputeDesc.myType = GL_UNSIGNED_BYTE;
    precomputeDesc.myClamp = GL_CLAMP_TO_BORDER;
    myAtmosphereTexturePrecomputeFrame = Framebuffer::Create(precomputeDesc.myWidth, precomputeDesc.myHeight);
    myAtmosphereTexturePrecomputeFrame->AddAttachment("Color", Texture::Create(precomputeDesc));

    myAtmosphereTexturePrecomputeMaterial = Material::Create("Data/SolarOdyssey/Shaders/Final.vs", "Data/SolarOdyssey/Shaders/AtmosphereTexture.fs");
    myAtmosphereTexturePrecomputeFrame->Bind();
    glClear(GL_COLOR_BUFFER_BIT);
    myAtmosphereTexturePrecomputeMaterial->Bind();
    Primitives::DrawFullscreenQuad();
    //
    //////////////////////////////////////////////////////////////////////////
}

SolarOdysseyRenderer::~SolarOdysseyRenderer()
{

}

void SolarOdysseyRenderer::Render(float delta)
{
#if !GE_FINAL
    //static ULONGLONG lastLoadTime = GetTickCount64(); // move this
    //if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('S'))
    //{
    //    const ULONGLONG tickCount = GetTickCount64();
    //    if (tickCount - lastLoadTime > 200)
    //    {
    //        Sleep(100); // Wait for a while to let the file system finish the file write.

    //        myAtmosphereTexturePrecomputeMaterial = Material::Create("Data/SolarOdyssey/Shaders/Final.vs", "Data/SolarOdyssey/Shaders/AtmosphereTexture.fs");

    //        myFinalMaterial = Material::Create("Data/SolarOdyssey/Shaders/Final.vs", "Data/SolarOdyssey/Shaders/Final.fs");
    //        myFinalMaterial->AddTexture("uColor", myColorFrame->myTextures["Color"]);
    //        myFinalMaterial->AddTexture("uDepth", myDepthFrame->myTextures["Depth"], 1);
    //        myFinalMaterial->AddTexture("uBlueNoise", Texture::Create("Data/GrumpyEngine/Textures/BlueNoise.png", GL_LINEAR, GL_REPEAT), 2);
    //        myFinalMaterial->AddTexture("uAtmospherePrecompute", myAtmosphereTexturePrecomputeFrame->myTextures["Color"], 3);
    //        myFinalMaterial->AddTexture("uWaveNormalsA", Texture::Create("Data/SolarOdyssey/Textures/WaveA.png", GL_NEAREST, GL_REPEAT), 4);
    //        myFinalMaterial->AddTexture("uWaveNormalsB", Texture::Create("Data/SolarOdyssey/Textures/WaveB.png", GL_NEAREST, GL_REPEAT), 5);
    //    }
    //    lastLoadTime = tickCount;
    //}

    //myAtmosphereTexturePrecomputeFrame->Bind();
    //glClear(GL_COLOR_BUFFER_BIT);
    //myAtmosphereTexturePrecomputeMaterial->Bind();
    //Primitives::DrawFullscreenQuad();
#endif

    //Lighting::ourLightBuffer.myDirectionalLight.myDirection = Math::GetForwardVector(Vec3(0.0f, Clock::GetCurrentTimeInSec(), 0.0f));

    RendererAPI::SetWireframeMode(myWireframeEnabled);
    //////////////////////////////////////////////////////////////////////////
    // Depth
    myDepthFrame->Bind();
    glDepthMask(true);
    glDepthFunc(GL_LESS);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (FrameData::Instance instance : Renderer::ourFrameData.myInstances)
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
    myDepthFrame->Unbind();
    //
    //////////////////////////////////////////////////////////////////////////
    // Color
    myColorFrame->Bind();
    glClear(GL_COLOR_BUFFER_BIT); 

    for (FrameData::Instance instance : Renderer::ourFrameData.myInstances)
    {
        const Mesh* mesh = instance.myMesh.get();
        const SPtr<Material> material = myWireframeEnabled ? Material::Create("Wireframe") : instance.myMaterial;
        const Mat4& transform = instance.myMatrix;

        material->Bind(RenderPass::Geometry);
        material->myShaders[RenderPass::Geometry]->BindMatrix4x4("uModelMatrix", transform);
        mesh->Render();
    }

    myColorFrame->Unbind();
    RendererAPI::SetWireframeMode(false);
    //
    //////////////////////////////////////////////////////////////////////////
    // Final
    glViewport(0, 0, (int)Renderer::ourRenderTargetDimensions.x, (int)Renderer::ourRenderTargetDimensions.y);
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myFinalMaterial->Bind();
    Primitives::DrawFullscreenQuad();

#if !GE_FINAL
    myFinalFrame->Unbind();
#endif
}

void SolarOdysseyRenderer::Resize(int width, int height)
{
    TextureDesc desc{};
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myFilter = GL_LINEAR;

    myColorFrame = Framebuffer::Create(width, height);
    desc.myAttachment = GL_COLOR_ATTACHMENT0;
    desc.myInternalFormat = GL_RGBA;
    desc.myFormat = GL_RGBA;
    desc.myType = GL_UNSIGNED_BYTE;
    desc.myClamp = GL_MIRRORED_REPEAT;
    myColorFrame->AddAttachment("Color", Texture::Create(desc));
    desc.myClamp = GL_CLAMP_TO_BORDER;

#if !GE_FINAL
    myFinalFrame = Framebuffer::Create(width, height);
    myFinalFrameTexture = Texture::Create(desc);
    myFinalFrame->AddAttachment("Color", myFinalFrameTexture);
#endif

    //////////////////////////////////////////////////////////////////////////

    myDepthFrame = Framebuffer::Create(width, height);
    desc.myAttachment = GL_DEPTH_ATTACHMENT;
    desc.myInternalFormat = GL_DEPTH24_STENCIL8;
    desc.myFormat = GL_DEPTH_STENCIL;
    desc.myType = GL_UNSIGNED_INT_24_8;
    myDepthFrame->AddAttachment("Depth", Texture::Create(desc));
    myColorFrame->AddAttachment("Depth", myDepthFrame->myTextures["Depth"]);

    myFinalMaterial = Material::Create("Data/SolarOdyssey/Shaders/Final.vs", "Data/SolarOdyssey/Shaders/Final.fs");
    myFinalMaterial->AddTexture("uColor", myColorFrame->myTextures["Color"]);
    myFinalMaterial->AddTexture("uDepth", myDepthFrame->myTextures["Depth"], 1);
    myFinalMaterial->AddTexture("uBlueNoise", Texture::Create("Data/GrumpyEngine/Textures/BlueNoise.png", GL_LINEAR, GL_REPEAT), 2);
    myFinalMaterial->AddTexture("uAtmospherePrecompute", myAtmosphereTexturePrecomputeFrame->myTextures["Color"], 3);
    myFinalMaterial->AddTexture("uWaveNormalsA", Texture::Create("Data/SolarOdyssey/Textures/WaveA.png", GL_NEAREST, GL_REPEAT), 4);
    myFinalMaterial->AddTexture("uWaveNormalsB", Texture::Create("Data/SolarOdyssey/Textures/WaveB.png", GL_NEAREST, GL_REPEAT), 5);
}

#if !GE_FINAL
void SolarOdysseyRenderer::RenderDebug()
{
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
    if (!ImGui::Begin("Solar Odyssey Renderer Settings"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Atmosphere Rendering");
    ImGui::DragFloat("Density Falloff", &myAtmosphereBuffer.myDensityFalloff, 0.1f);
    ImGui::DragFloat("Num In Scatter Points", &myAtmosphereBuffer.myNumInScatterPoints);
    ImGui::DragFloat("Num Optical Depth Points", &myAtmosphereBuffer.myNumOpticalDepthPoints);
    ImGui::DragFloat("Planet Radius", &myAtmosphereBuffer.myPlanetRadius, 0.1f);
    ImGui::DragFloat("Atmosphere Height", &myAtmosphereBuffer.myAtmosphereHeight, 0.1f);
    myAtmosphereBuffer.myAtmosphereRadius = myAtmosphereBuffer.myPlanetRadius + myAtmosphereBuffer.myAtmosphereHeight;
    ImGui::Text("Atmosphere Radius %f", myAtmosphereBuffer.myAtmosphereRadius);
    ImGui::DragFloat3("Planet Center", (float*)&myAtmosphereBuffer.myPlanetCenter, 0.1f);
    ImGui::DragFloat3("Wavelengths", (float*)&myWaveLengths);
    ImGui::DragFloat("Scatter Strength", &myAtmosphereBuffer.myScatteringStrength, 0.1f);
    ImGui::DragFloat("Intensity", &myAtmosphereBuffer.myIntensity, 0.1f);
    ImGui::DragFloat("Dither Strength", &myAtmosphereBuffer.myDitherStrength, 0.1f);
    ImGui::DragFloat("Dither Scale", &myAtmosphereBuffer.myDitherScale, 0.1f);

    ImGui::Separator();

    ImGui::Text("Water Rendering");
    ImGui::ColorEdit4("Specular Color", (float*)&myWaterBuffer.mySpecularColor);
    ImGui::ColorEdit4("Water Color1", (float*)&myWaterBuffer.myWaterColor1);
    ImGui::ColorEdit4("Water Color2", (float*)&myWaterBuffer.myWaterColor2);
    ImGui::DragFloat("Depth Multiplier", &myWaterBuffer.myDepthMultiplier, 0.1f);
    ImGui::DragFloat("Alpha Multiplier", &myWaterBuffer.myAlphaMultiplier, 0.1f);
    ImGui::DragFloat("Smoothness", &myWaterBuffer.mySmoothness, 0.01f);
    ImGui::DragFloat("Wave Speed", &myWaterBuffer.myWaveSpeed, 0.01f);
    ImGui::DragFloat("Wave Normal Scale", &myWaterBuffer.myWaveNormalScale, 0.01f);
    ImGui::DragFloat("Wave Strength", &myWaterBuffer.myWaveStrength, 0.01f);

    ImGui::Separator();

    ImGui::Text("Misc");
    ImGui::Checkbox("Wireframe", &myWireframeEnabled);

    CalculateCoefficients();
    myAtmosphereUniformBuffer->SetData(&myAtmosphereBuffer);
    myWaterUniformBuffer->SetData(&myWaterBuffer);

    ImGui::End();
}
#endif

void SolarOdysseyRenderer::CalculateCoefficients()
{
    float scatterR = pow(400.0f / myWaveLengths.x, 4.0f) * myAtmosphereBuffer.myScatteringStrength;
    float scatterG = pow(400.0f / myWaveLengths.y, 4.0f) * myAtmosphereBuffer.myScatteringStrength;
    float scatterB = pow(400.0f / myWaveLengths.z, 4.0f) * myAtmosphereBuffer.myScatteringStrength;
    myAtmosphereBuffer.myScatterCoeffecients = Vec3(scatterR, scatterG, scatterB);
}

#if !GE_FINAL
SPtr<Texture> SolarOdysseyRenderer::GetFrameTexture()
{
    return myFinalFrameTexture;
}
#endif
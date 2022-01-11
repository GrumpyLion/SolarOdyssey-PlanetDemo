#include "pch.h"

#include "Common/Window.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Primitives.h"
#include "Graphics/RendererGlobals.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/Renderer.h"
#include "Graphics/CameraMan.h"
#include "Graphics/Lighting.h"

#include "SolarOdyssey.h"
#include "SolarSystem/Planet.h"

#include "SolarOdysseyRenderer.h"

#if !GE_FINAL
#include <imgui.h>
#include "Editor/Editor.h"
#endif

SolarOdysseyRenderer::SolarOdysseyRenderer()
{
    mySkySphereMesh = Mesh::Create("Data/SolarOdyssey/Models/Sun.glb");
    mySkySphereMaterial = Material::Create("Data/GrumpyEngine/Shaders/MotionVectorsBoundary.vs", "Data/GrumpyEngine/Shaders/MotionVectors.fs");
    mySkySphereMaterial->AddShaderPass("Data/GrumpyEngine/Shaders/Centered.vs", "Data/GrumpyEngine/Shaders/Depth.fs", RenderPass::Depth);

    myGridMaterial = Material::Create("Data/GrumpyEngine/Shaders/Grid.vs", "Data/GrumpyEngine/Shaders/Grid.fs");

    CalculateCoefficients();
    myAtmosphereUniformBuffer = UniformBuffer::Create(sizeof(AtmosphereBuffer), (uint)UniformBufferIndexes::AtmosphereBuffer, "Atmosphere Buffer", &myAtmosphereBuffer, GL_STATIC_DRAW);
    myWaterUniformBuffer = UniformBuffer::Create(sizeof(WaterBuffer), (uint)UniformBufferIndexes::WaterBuffer, "Water Buffer", &myWaterBuffer, GL_STATIC_DRAW);

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
    Texture::Track(myAtmosphereTexturePrecomputeFrame->myTextures["Color"], "Atmosphere Precompute");
    myAtmosphereTexturePrecomputeFrame->Finalize();

    myAtmosphereTexturePrecomputeMaterial = Material::Create("Data/GrumpyEngine/Shaders/Composite.vs", "Data/SolarOdyssey/Shaders/AtmosphereTexture.fs");
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
    RendererAPI::SetWireframeMode(myWireframeEnabled);

    //////////////////////////////////////////////////////////////////////////
    // Depth

    myDepthFrame->Bind();

    glCullFace(GL_BACK);
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

    myFoliageManager.Render(delta, RenderPass::Depth);

    glDepthFunc(GL_EQUAL);
    glDepthMask(false);
    myDepthFrame->Unbind();

    //
    //////////////////////////////////////////////////////////////////////////
    // Shadows

    if (myShadowsEnabled)
    {
        Lighting::ourDirectionalLight->Bind();

        for (FrameData::Instance instance : Renderer::ourFrameData.myInstances)
        {
            const Mesh* mesh = instance.myMesh.get();
            const Material* material = instance.myMaterial.get();
            const Mat4& transform = instance.myMatrix;

            material->Bind(RenderPass::Shadows);
            material->myShaders[RenderPass::Shadows]->BindMatrix4x4("uModelMatrix", transform);
            mesh->Render();
        }

        myFoliageManager.Render(delta, RenderPass::Shadows);

        Lighting::ourDirectionalLight->Unbind();

        if (myShadowBlurEnabled)
            Lighting::ourDirectionalLight->Blur();
    }
    else
        Lighting::ourDirectionalLight->Clear();

    //
    //////////////////////////////////////////////////////////////////////////
    // Color

    myColorFrame->Bind();
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    glClear(GL_COLOR_BUFFER_BIT); 


#if !GE_FINAL
    if (myGridEnabled)
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        myGridMaterial->Bind();
		static GLuint vao = 0;
		glCreateVertexArrays(1, &vao);
		glBindVertexArray(vao);
        glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
    }
#endif

    for (FrameData::Instance instance : Renderer::ourFrameData.myInstances)
    {
        const Mesh* mesh = instance.myMesh.get();
        const SPtr<Material> material = myWireframeEnabled ? Material::Create("Wireframe") : instance.myMaterial;
        const Mat4& transform = instance.myMatrix;

        material->Bind(RenderPass::Geometry);
        material->myShaders[RenderPass::Geometry]->BindMatrix4x4("uModelMatrix", transform);
        mesh->Render();
    }

    myFoliageManager.Render(delta, RenderPass::Geometry);

    myColorFrame->Unbind();
    RendererAPI::SetWireframeMode(false);

    //
    //////////////////////////////////////////////////////////////////////////
    // Motion Vectors

    myMotionVectorsFrame->Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    for (FrameData::Instance instance : Renderer::ourFrameData.myInstances)
    {
        Mesh* mesh = instance.myMesh.get();
        const SPtr<Material> material = instance.myMaterial;
        const Mat4& transform = instance.myMatrix;

        material->Bind(RenderPass::MotionVectors);
        material->myShaders[RenderPass::MotionVectors]->BindMatrix4x4("uModelMatrix", transform);
        material->myShaders[RenderPass::MotionVectors]->BindMatrix4x4("uPrevModelMatrix", mesh->myPrevMatrix);
        mesh->Render();

        mesh->myPrevMatrix = transform;
    }

    myFoliageManager.Render(delta, RenderPass::MotionVectors);

    myMotionVectorsFrame->Unbind();

    //
    //////////////////////////////////////////////////////////////////////////
    // Atmosphere

    myAtmosphereFrame->Bind();
    glClear(GL_COLOR_BUFFER_BIT);

    myAtmosphereMaterial->Bind();

    Primitives::DrawFullscreenQuad();

    myAtmosphereFrame->Unbind();

    //
    //////////////////////////////////////////////////////////////////////////
    // Motion Blur

    if (myMotionBlurEnabled)
    {
        myMotionBlurFrame->Bind();
        glClear(GL_COLOR_BUFFER_BIT);

        myMotionBlurMaterial->Bind();

        Primitives::DrawFullscreenQuad();

        myMotionBlurFrame->Unbind();
    }

    //
    //////////////////////////////////////////////////////////////////////////
    // Final

    glViewport(0, 0, (int)Renderer::ourRenderTargetDimensions.x, (int)Renderer::ourRenderTargetDimensions.y);
    glDepthFunc(GL_LESS);
    glDepthMask(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    myFinalMaterial->Bind();
    Primitives::DrawFullscreenQuad();
}

void SolarOdysseyRenderer::Resize(int width, int height)
{
    TextureDesc desc{};
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myFilter = GL_LINEAR;

    myColorFrame = Framebuffer::Create(width, height);
    myMotionVectorsFrame = Framebuffer::Create(width, height);
    myMotionBlurFrame = Framebuffer::Create(width, height);
    myAtmosphereFrame = Framebuffer::Create(width, height);
    myDepthFrame = Framebuffer::Create(width, height);

    desc.myAttachment = GL_COLOR_ATTACHMENT0;
    desc.myInternalFormat = GL_RGBA8;
    desc.myType = GL_UNSIGNED_BYTE;
    desc.myClamp = GL_MIRRORED_REPEAT;
    
    myColorFrame->AddAttachment("Color", Texture::Create(desc));
    Texture::Track(myColorFrame->myTextures["Color"], "Color");

    myMotionBlurFrame->AddAttachment("Color", Texture::Create(desc));
    Texture::Track(myMotionBlurFrame->myTextures["Color"], "Motion Blur");

    myAtmosphereFrame->AddAttachment("Color", Texture::Create(desc));
    Texture::Track(myAtmosphereFrame->myTextures["Color"], "Atmosphere Step");

    desc.myClamp = GL_CLAMP_TO_BORDER;
    desc.myInternalFormat = GL_RGB16F;
    myMotionVectorsFrame->AddAttachment("Color", Texture::Create(desc));
    Texture::Track(myMotionVectorsFrame->myTextures["Color"], "MotionVectors");

    desc.myAttachment = GL_COLOR_ATTACHMENT1;
    myColorFrame->AddAttachment("Normals", Texture::Create(desc));
    Texture::Track(myColorFrame->myTextures["Normals"], "Normals");

    desc.myAttachment = GL_COLOR_ATTACHMENT2;
    myColorFrame->AddAttachment("FragPos", Texture::Create(desc));
    Texture::Track(myColorFrame->myTextures["FragPos"], "FragPos");

    //////////////////////////////////////////////////////////////////////////

    desc.myAttachment = GL_DEPTH_ATTACHMENT;
    desc.myInternalFormat = GL_DEPTH24_STENCIL8;
    desc.myType = GL_UNSIGNED_INT_24_8;
    
    myDepthFrame->AddAttachment("Depth", Texture::Create(desc));
    Texture::Track(myDepthFrame->myTextures["Depth"], "Depth");

    myColorFrame->AddAttachment("Depth", myDepthFrame->myTextures["Depth"]);
    myMotionVectorsFrame->AddAttachment("Depth", myDepthFrame->myTextures["Depth"]);
    
    myDepthFrame->Finalize();
    myMotionVectorsFrame->Finalize();
    myColorFrame->Finalize();
    myAtmosphereFrame->Finalize();
    myMotionBlurFrame->Finalize();

    //////////////////////////////////////////////////////////////////////////

    myAtmosphereMaterial = Material::Create("Data/SolarOdyssey/Shaders/Atmosphere.vs", "Data/SolarOdyssey/Shaders/Atmosphere.fs");
    myAtmosphereMaterial->AddTexture("uColor", myColorFrame->myTextures["Color"]);
    myAtmosphereMaterial->AddTexture("uDepth", myDepthFrame->myTextures["Depth"], 1);
    myAtmosphereMaterial->AddTexture("uBlueNoise", Texture::Create("Data/GrumpyEngine/Textures/BlueNoise.png", GL_LINEAR, GL_REPEAT), 2);
    myAtmosphereMaterial->AddTexture("uAtmospherePrecompute", myAtmosphereTexturePrecomputeFrame->myTextures["Color"], 3);
    myAtmosphereMaterial->AddTexture("uWaveNormalsA", Texture::Create("Data/SolarOdyssey/Textures/WaveA.png", GL_NEAREST, GL_REPEAT), 4);
    myAtmosphereMaterial->AddTexture("uWaveNormalsB", Texture::Create("Data/SolarOdyssey/Textures/WaveB.png", GL_NEAREST, GL_REPEAT), 5);

    myMotionBlurMaterial = Material::Create("Data/GrumpyEngine/Shaders/MotionBlur.vs", "Data/GrumpyEngine/Shaders/MotionBlur.fs");
    myMotionBlurMaterial->AddTexture("uColor", myAtmosphereFrame->myTextures["Color"]);
    myMotionBlurMaterial->AddTexture("uMotionVectors", myMotionVectorsFrame->myTextures["Color"], 1);

    myFinalMaterial = Material::Create("Data/GrumpyEngine/Shaders/Composite.vs", "Data/GrumpyEngine/Shaders/Composite.fs");
    myFinalMaterial->AddTexture("uColor", myMotionBlurFrame->myTextures["Color"]);
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
    ImGui::Checkbox("Grid", &myGridEnabled);
    ImGui::Checkbox("Shadows", &myShadowsEnabled);
    ImGui::Checkbox("Shadows Blur", &myShadowBlurEnabled);

    if (ImGui::Checkbox("Motion Blur", &myMotionBlurEnabled))
    {
        if (myMotionBlurEnabled)
            myFinalMaterial->AddTexture("uColor", myMotionBlurFrame->myTextures["Color"]);
        else
            myFinalMaterial->AddTexture("uColor", myAtmosphereFrame->myTextures["Color"]);
    }

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
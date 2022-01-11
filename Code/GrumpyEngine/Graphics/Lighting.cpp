#include "pch.h"

#include "Lighting.h"
#include "Renderer.h"
#include "CameraMan.h"
#include "Primitives.h"
#include "RendererGlobals.h"
#include "CameraMan.h"

//Ref<SpotLight> SpotLight::Create(int width, int height)
//{
//    return Ref<SpotLight>::Create(width, height);
//}
//
//SpotLight::SpotLight(int width, int height)
//{
//    myWidth = width;
//    myHeight = height;
//
//    //myDepthMaterial = Material::Create("SpotLight Depth"); // TODO fix all
//    myBuffer = UniformBuffer::Create(sizeof(Buffer), UniformBufferIndexes::SpotLightShadowBuffer);
//
//    myFramebuffer = Framebuffer::Create(width, height);
//
//    TextureDesc desc{};
//    desc.myWidth = width;
//    desc.myHeight = height;
//    desc.myFormat = TextureFormat::Depth;
//    desc.myAttachment = TextureAttachment::Depth;
//    //myDepthTexture = Texture::Create(desc);
//    myDepthTexture->BindAsFramebufferTexture();
//    myFramebuffer->Unbind();
//}
//
//void SpotLight::RenderDepth()
//{
//    Buffer buffer{};
//    float nearPlane = 0.1f;
//    buffer.myProjectionMatrix = glm::perspective(glm::radians(mySpotAngle * 2), 1.0f / 1.0f, nearPlane, myFarPlane);
//
//    buffer.myViewMatrix = Mat4(1.0);
//    buffer.myViewMatrix *= Math::Rotate(myRotation * Vec3(1, -1, 1));
//    buffer.myViewMatrix *= Math::Translate(-myPosition);
//
//    myBuffer->SetData(&buffer);
//    myBuffer->Bind();
//    myFramebuffer->Bind();
//    glDepthMask(true);
//    glDepthFunc(GL_LESS);
//
//	glClear(GL_DEPTH_BUFFER_BIT);
//	// TODO rendering is missing
//
//    myFramebuffer->Unbind();
//    glDepthMask(false);
//    glDepthFunc(GL_EQUAL);
//}
//
////////////////////////////////////////////////////////////////////////////
//
SPtr<DirectionalLight> DirectionalLight::Create(int width, int height)
{
    return MakeShared<DirectionalLight>(width, height);
}

DirectionalLight::DirectionalLight(int width, int height)
    : myWidth(width)
    , myHeight(height)
{
    myBuffer = UniformBuffer::Create(sizeof(Buffer), (uint)UniformBufferIndexes::DirectionalLightShadowBuffer);
    myBlurMaterial = Material::Create("Data/GrumpyEngine/Shaders/Blur.vs", "Data/GrumpyEngine/Shaders/Blur.fs");

    myBlurFrame = Framebuffer::Create(width, height);
    myDepthFrame = Framebuffer::Create(width, height);

    TextureDesc desc{};
    desc.myWidth = width;
    desc.myHeight = height;
    desc.myBorderColor = Vec4(1.0f);
    desc.myClamp = GL_CLAMP_TO_BORDER;
    desc.myFilter = GL_LINEAR;
    desc.myAttachment = GL_COLOR_ATTACHMENT0;
    desc.myInternalFormat = GL_RG32F;
    desc.myType = GL_FLOAT;

    myDepthFrame->AddAttachment("Color", Texture::Create(desc));
    Texture::Track(myDepthFrame->myTextures["Color"], "Directional Light Depth");
    
    myBlurFrame->AddAttachment("Color", Texture::Create(desc));
    Texture::Track(myBlurFrame->myTextures["Color"], "Directional Light Depth Blurred");

    desc.myAttachment = GL_DEPTH_ATTACHMENT;
    desc.myInternalFormat = GL_DEPTH24_STENCIL8;
    desc.myType = GL_UNSIGNED_INT_24_8;

    myDepthFrame->AddAttachment("Depth", Texture::Create(desc));
    Texture::Track(myDepthFrame->myTextures["Depth"], "DD Depth");
    
    myBlurFrame->Finalize();
    myDepthFrame->Finalize();
}

void DirectionalLight::Bind()
{
    Buffer buffer{};
    float nearPlane = 1.0f;
    float farPlane = 40.0f;
    float scaleX = 10.0f;
    float scaleY = 10.0f;
    Vec3 position = Vec3(CameraMan::ourPosition.x - 15.0f, CameraMan::ourPosition.y, CameraMan::ourPosition.z);

    buffer.myProjectionMatrix = glm::ortho(-myWidth / 2.0f, myWidth / 2.0f, -myHeight / 2.0f, myHeight / 2.0f, nearPlane, farPlane);

    Mat4 direction = Math::LookAt(Vec3(-1, 0, 0), Vec3(0, 0, 0), Vec3(0, 1, 0));
    Mat4 view = Math::Scale(Vec3(scaleX, scaleY, 1.0)) * direction;
    Vec4 spos = view * Vec4(position, 1.0);
    Vec3 off = -glm::round(Vec3(spos));
    Mat4 offset = Math::Translate(off);
    buffer.myViewMatrix = offset * view;


    myBuffer->SetData(&buffer);
    myBuffer->Bind();
    myDepthFrame->Bind();
    glDepthMask(true);
    glDepthFunc(GL_LESS);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DirectionalLight::Unbind()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDepthMask(false);
    glDepthFunc(GL_EQUAL);
}

void DirectionalLight::Blur()
{
    myBlurFrame->Bind();

    myBlurMaterial->AddTexture("uTexture", myDepthFrame->myTextures["Color"], 0);
    myBlurMaterial->Bind();
    myBlurMaterial->myShaders[RenderPass::Geometry]->BindVector2f("uBlur", Vec2(1.0f / (myWidth * 0.8f), 0.0f));

    Primitives::DrawFullscreenQuad();
    myBlurFrame->Unbind();

    myDepthFrame->Bind();
    glDisable(GL_DEPTH_TEST);
    myBlurMaterial->AddTexture("uTexture", myBlurFrame->myTextures["Color"], 0);
    myBlurMaterial->Bind();
    myBlurMaterial->myShaders[RenderPass::Geometry]->BindVector2f("uBlur", Vec2(0.0f, 1.0f / (myHeight * 0.8f)));

    Primitives::DrawFullscreenQuad();
    myDepthFrame->Unbind();
    glEnable(GL_DEPTH_TEST);
}

void DirectionalLight::Clear()
{
    myDepthFrame->Bind();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myDepthFrame->Unbind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////

namespace Lighting
{
    //Ref<SpotLight> ourSpotLight{};
    SPtr<DirectionalLight> ourDirectionalLight{};

    LightBuffer ourLightBuffer{};
    SPtr<UniformBuffer> ourLightUniformBuffer{};
}

void Lighting::Initialize()
{
    ourLightBuffer.myDirectionalLight.myColor = Vec4(1.0f, 0.6f, 0.15f, 1.0f);
    ourLightBuffer.myDirectionalLight.myDirection = Vec3(1.0f, 0.0f, 0.0f);
    ourLightUniformBuffer = UniformBuffer::Create(sizeof(LightBuffer), (uint)UniformBufferIndexes::LightBuffer, "Light Buffer", &ourLightBuffer);
    ourDirectionalLight = DirectionalLight::Create(1024, 1024);
    //ourSpotLight = SpotLight::Create(256, 256);
}

void Lighting::Render()
{
    //ourSpotLight->myDepthTexture->Bind(TextureIndexes::SpotLightDepth);
    //ourDirectionalLight->RenderDepth();
    ourDirectionalLight->myDepthFrame->myTextures["Color"]->Bind((uint)TextureIndexes::DirectionalLightDepth);

    ourLightUniformBuffer->SetData(&ourLightBuffer);
    ourLightUniformBuffer->Bind();
    ourLightBuffer.mySpotLightCount = 0;
}

void Lighting::SetDirectionalLight(const DirectionalLightData& light, const Vec3& rotation)
{
    ourLightBuffer.myDirectionalLight = light;
    //ourDirectionalLight->myRotation = rotation;
}

void Lighting::AddSpotLight(const SpotLightData& light, const Vec3& rotation, bool shadows, float spotAngle)
{
    if (ourLightBuffer.mySpotLightCount >= ourMaxSpotLightCount)
        return;

    ourLightBuffer.mySpotLights[ourLightBuffer.mySpotLightCount++] = light;
}
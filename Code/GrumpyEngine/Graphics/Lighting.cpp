#include "pch.h"

#include "Lighting.h"
#include "Renderer.h"
#include "CameraMan.h"
#include "RendererGlobals.h"

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
//Ref<DirectionalLight> DirectionalLight::Create(int width, int height)
//{
//    return Ref<DirectionalLight>::Create(width, height);
//}
//
//DirectionalLight::DirectionalLight(int width, int height)
//    : myWidth(width)
//    , myHeight(height)
//{
//    //myDepthMaterial = Material::Create("Directional Depth");
//    myBuffer = UniformBuffer::Create(sizeof(Buffer), UniformBufferIndexes::DirectionalLightShadowBuffer);
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
//void DirectionalLight::RenderDepth()
//{
//    Buffer buffer{};
//    float nearPlane = 0.1f;
//    float farPlane = 500.0f;
//    buffer.myProjectionMatrix = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, nearPlane, farPlane);
//
//    //myBuffer.myViewMatrix = Mat4(1.0);
//    //myBuffer.myViewMatrix *= Math::Rotate(myRotation);
//    buffer.myViewMatrix = glm::lookAt(glm::vec3(.0f, .0f, -150.0f),
//        glm::vec3(0.0f, 0.0f, 0.0f),
//        glm::vec3(0.0f, 1.0f, 0.0f));
//
//    myBuffer->SetData(&buffer);
//    myBuffer->Bind();
//    myFramebuffer->Bind();
//    glDepthMask(true);
//    glDepthFunc(GL_LESS);
//
//    glClear(GL_DEPTH_BUFFER_BIT);
//    // TODO rendering is missing
//
//    myFramebuffer->Unbind();
//    glDepthMask(false);
//    glDepthFunc(GL_EQUAL);
//}

//////////////////////////////////////////////////////////////////////////

namespace Lighting
{
    //Ref<SpotLight> ourSpotLight{};
    //Ref<DirectionalLight> ourDirectionalLight{};

    LightBuffer ourLightBuffer{};
    SPtr<UniformBuffer> ourLightUniformBuffer{};
}

void Lighting::Initialize()
{
    ourLightBuffer.myDirectionalLight.myColor = Vec4(1.0f, 0.6f, 0.15f, 1.0f);
    ourLightBuffer.myDirectionalLight.myDirection = Vec3(1.0f, 0.0f, 0.0f);
    ourLightUniformBuffer = UniformBuffer::Create(sizeof(LightBuffer), (uint)UniformBufferIndexes::LightBuffer, &ourLightBuffer);
    //ourDirectionalLight = DirectionalLight::Create(1024, 1024);
    //ourSpotLight = SpotLight::Create(256, 256);
}

void Lighting::Render()
{
    //ourSpotLight->myDepthTexture->Bind(TextureIndexes::SpotLightDepth);
    //ourDirectionalLight->RenderDepth();
    //ourDirectionalLight->myDepthTexture->Bind(TextureIndexes::DirectionalLightDepth);

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
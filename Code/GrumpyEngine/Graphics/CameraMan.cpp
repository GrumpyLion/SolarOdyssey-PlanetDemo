#include "pch.h"

#include "CameraMan.h"
#include "RendererGlobals.h"
#include "UniformBuffer.h"
#include "Renderer.h"

struct CameraBuffer
{
    Mat4 myProjectionMatrix = Mat4(1.0f);
    Mat4 myPrevProjectionMatrix = Mat4(1.0f);
    Mat4 myViewMatrix = Mat4(1.0f);
    Mat4 myViewMatrixCentered = Mat4(1.0f);
    Mat4 myPrevViewMatrix = Mat4(1.0f);
    Mat4 myPrevViewMatrixCentered = Mat4(1.0f);
    Mat4 myInvProjViewMatrix = Mat4(1.0f);
    Mat4 myInvProjectionMatrix = Mat4(1.0f);
    Mat4 myInvViewMatrix = Mat4(1.0f);
    Vec4 myResolutionAndTime = Vec4(0.0f);
    Vec4 myCameraPos = Vec4(0.0f);
    Vec4 myCameraDir = Vec4(0.0f);
    Vec4 myNearFarFOV = Vec4(0.0f);
};

namespace CameraMan
{
    Vec3 ourPosition = Vec3(0.0f);
    Vec3 ourDirection = Vec3(0.0f);
    Vec3 ourRotation = Vec3(0.0f);
    Mat4 ourProjectionMatrix = Mat4(1.0f);
    Mat4 ourViewMatrix = Mat4(1.0f);
    Mat4 ourViewMatrixCentered = Mat4(1.0f);
    Mat4 ourInverseProjectionMatrix = Mat4(1.0f);
    Mat4 ourInverseViewMatrix = Mat4(1.0f);
    Mat4 ourInverseProjViewMatrix = Mat4(1.0f);

    Mat4 locPrevProjectionMatrix = Mat4(1.0f);
    Mat4 locPrevViewMatrix = Mat4(1.0f);
    Mat4 locPrevViewMatrixCentered = Mat4(1.0f);

    int ourCurrentCamera = 0;
#if !GE_FINAL
    bool ourUseEditorCamera = false;
#endif

    SPtr<UniformBuffer> locCameraBuffer;
    constexpr int locMaxCameraCount = 64;
    CameraData locCameras[locMaxCameraCount];
}

void CameraMan::Initialize()
{
    locCameraBuffer = UniformBuffer::Create(sizeof(CameraBuffer), (uint)UniformBufferIndexes::CameraBuffer, "Camera Buffer");
}

void CameraMan::Render(float delta)
{
#if !GE_FINAL
    CameraData& camera = locCameras[ourUseEditorCamera ? locMaxCameraCount - 1 : ourCurrentCamera];
#else
    CameraData& camera = locCameras[ourCurrentCamera];
#endif

    ourProjectionMatrix = glm::perspective(Math::ToRadians(camera.myFOV), Renderer::ourRenderTargetDimensions.x / Renderer::ourRenderTargetDimensions.y, camera.myNear, camera.myFar);
    ourInverseProjectionMatrix = Math::Inverse(ourProjectionMatrix);
    if (camera.myInterpolate)
    {
        ourPosition = Math::Damp(ourPosition, camera.myPosition, camera.myLamdba, delta);
        ourRotation = Math::Damp(ourRotation, camera.myRotation, camera.myLamdba, delta);
    }
    else
    {
        ourPosition = camera.myPosition;
        ourRotation = camera.myRotation;
    }

    if (camera.myUseGivenMatrix)
        ourViewMatrix = camera.myViewMatrix;
    else
        ourViewMatrix = glm::lookAt(ourPosition, ourPosition + Math::GetForwardVector(ourRotation), Vec3(0.0f, 1.0f, 0.0f));

    ourViewMatrixCentered = glm::lookAt(Vec3(0.0f), Math::GetForwardVector(ourRotation), Vec3(0.0f, 1.0f, 0.0f));

    Mat4 inverse = glm::inverse(ourViewMatrix);
    ourPosition = Vec3(inverse[3]);
    ourDirection = Vec3(inverse[2]);

    ourInverseViewMatrix = Math::Inverse(ourViewMatrix);
    ourInverseProjectionMatrix = Math::Inverse(ourProjectionMatrix);
    ourInverseProjViewMatrix = Math::Inverse(ourProjectionMatrix * ourViewMatrix);

    CameraBuffer buffer{};
    buffer.myCameraPos = Vec4(ourPosition, 0.0f);
    buffer.myCameraDir = Vec4(Math::Normalize(ourDirection), 0.0f);
    buffer.myViewMatrix = ourViewMatrix;
    buffer.myViewMatrixCentered = ourViewMatrixCentered;
    buffer.myProjectionMatrix = ourProjectionMatrix;
    buffer.myPrevProjectionMatrix = locPrevProjectionMatrix;
    buffer.myPrevViewMatrix = locPrevViewMatrix;
    buffer.myPrevViewMatrixCentered = locPrevViewMatrixCentered;
    buffer.myInvProjViewMatrix = ourInverseProjViewMatrix;
    buffer.myInvProjectionMatrix = ourInverseProjectionMatrix;
    buffer.myInvViewMatrix = ourInverseViewMatrix;
    buffer.myResolutionAndTime = Vec4(Renderer::ourRenderTargetDimensions.x, Renderer::ourRenderTargetDimensions.y, Clock::GetCurrentTimeInSec(), Clock::GetCurrentTimeInSec());
    buffer.myNearFarFOV = Vec4(camera.myNear, camera.myFar, camera.myFOV, 0.0f);
    locCameraBuffer->SetData(&buffer);
}

void CameraMan::PostRender()
{
    locPrevProjectionMatrix = ourProjectionMatrix;
    locPrevViewMatrix = ourViewMatrix;
    locPrevViewMatrixCentered = ourViewMatrixCentered;
}

void CameraMan::SetCameraData(uint index, const CameraData& cameraData)
{
    if (index >= locMaxCameraCount)
        return;

    locCameras[index] = cameraData;
}
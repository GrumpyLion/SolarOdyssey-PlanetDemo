#pragma once

namespace CameraMan
{
    void Initialize();
    void Render(float delta);
    void PostRender();

    struct CameraData
    {
        float myFOV = 60.0f;
        float myNear = 0.1f;
        float myFar = 100.0f;

        Mat4 myViewMatrix = Mat4(1.0f);
        Vec3 myPosition = Vec3(0.0f);
        Vec3 myRotation = Vec3(0.0f);

        bool myInterpolate;
        bool myUseGivenMatrix;
        float myLamdba;
    };

    // void SetTarget(); // TODO implement

    void SetCameraData(uint index, const CameraData& cameraData);

    extern Vec3 ourPosition;
    extern Vec3 ourDirection;
    extern Vec3 ourRotation;
    extern Mat4 ourProjectionMatrix;
    extern Mat4 ourViewMatrix;
    extern Mat4 ourInverseProjectionMatrix;
    extern Mat4 ourInverseViewMatrix;
    extern Mat4 ourInverseProjViewMatrix;
    extern int ourCurrentCamera;
#if !GE_FINAL
    extern bool ourUseEditorCamera;
#endif
};
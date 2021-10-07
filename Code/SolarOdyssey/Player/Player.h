#pragma once

#include "Audio/AudioManager.h"
#include "Physics/Rigidbody.h"
#include "Physics/CharacterController.h"
#include "Graphics/CameraMan.h"

struct Player
{
    Player();
    ~Player();

    void Update(float delta);
    void Render(float delta);


#if !GE_FINAL
    void DebugDraw();
#endif

    void CalculateSphericalMovement(float delta);
    void CalculateGravity(float delta);
    void ProcessInput(float delta);

    float myMouseSpeed = 10.0f;
    float myMovementSpeed = 4.0f;
    float mySprintSpeed = 7.0f;
    float myJumpForce = 100.0f;
    float myHeadBobStrenght = 20.0f;
    float myLastTimeStepSoundPlayed = 0.0f;

    bool myCameraActive = true;
    bool myRenderDebug = true;

    Vec3 myHeadBob = Vec3(0.0f);
    Vec3 myGravity = Vec3(0.0f);
    Vec3 myGravityDir = Vec3(0.0f);
    Vec3 myRotation = Vec3(0.0f);
    Vec3 myPosition = Vec3(0.0f);
    Vec3 myPoleDirection = Vec3(0, 1.0f, 0);
    Vec3 myPlanetAlignedForward = Vec3(0.0f);
    Vec3 mySideways = Vec3(0.0f);
    Vec3 myVelocity = Vec3(0.0f);
    Vec3 myGravityVelocity = Vec3(0.0f);

    AudioHandle myStepSound;
    UPtr<CharacterController> myController;
    CameraMan::CameraData myCameraData{};
};
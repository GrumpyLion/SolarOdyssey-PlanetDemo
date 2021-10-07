#pragma once

#include <PxPhysicsAPI.h>

struct CharacterController
{
    CharacterController(physx::PxControllerDesc& description);
    ~CharacterController();

    void Update(float delta);

    void SetGravity(const Vec3& gravity);
    void SetUp(const Vec3& up);
    void SetVelocity(const Vec3& dir);
    void Jump(const Vec3& dir);
    Vec3 GetPosition() const;


    physx::PxController* myController = nullptr;
    Vec3 myVelocity = Vec3(0.0f);
    Vec3 myGravity = Vec3(0.0f, 0.81f, 0.0f);
    Vec3 myFallingVelocity = Vec3(0.0f);

    bool myIsGrounded = false;
    bool myIsCeiled = false;
};
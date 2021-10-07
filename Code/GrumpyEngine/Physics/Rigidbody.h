#pragma once

#include <PxPhysicsAPI.h>

struct RigidbodyDynamic
{
    RigidbodyDynamic(float mass, const physx::PxGeometry& geometry, Vec3 position = Vec3(0.0f), Quat rotation = Quat());
    RigidbodyDynamic(const physx::PxGeometry& geometry, Vec3 position = Vec3(0.0f), Quat rotation = Quat());

    ~RigidbodyDynamic();

    void SetGravity(Vec3 gravity);
    void SetVelocity(Vec3 velocity);
    void ApplyForce(Vec3 force);
    void SetKinematic(bool kinematic);

    Vec3 GetPosition();
    Quat GetRotation();
    Vec3 GetVelocity();

    physx::PxRigidDynamic* myRigidbody{};
    physx::PxMaterial* myPhysicsMaterial{};
};

//////////////////////////////////////////////////////////////////////////

struct RigidbodyStatic
{
    RigidbodyStatic(const physx::PxGeometry& geometry, Vec3 position = Vec3(0.0f), Quat rotation = Quat());

    ~RigidbodyStatic();

    Vec3 GetPosition();
    
    physx::PxRigidStatic* myRigidbody{};
    physx::PxMaterial* myPhysicsMaterial{};
};
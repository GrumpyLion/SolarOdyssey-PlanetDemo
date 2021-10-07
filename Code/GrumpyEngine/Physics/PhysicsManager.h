#pragma once

#include <PxPhysicsAPI.h>

struct RayCastHit
{
    bool myHit = false;
    bool myIsGhost = false;
    Vec3 myNormal = Vec3(0.0f);
    Vec3 myPosition = Vec3(0.0f);
};

namespace PhysicsManager
{
    void Initialize();
    void Shutdown();

    void Update(float delta);

    RayCastHit RayCast(const Vec3& origin, const Vec3& unitDir, const float maxDistance);

    //////////////////////////////////////////////////////////////////////////

    extern bool ourStopPhysics;
    extern Vec3 ourGravity;

    extern physx::PxPhysics* ourPhysics;
    extern physx::PxScene* ourScene;
    extern physx::PxControllerManager* ourControllerManager;
    extern physx::PxCooking* ourCooking;
};
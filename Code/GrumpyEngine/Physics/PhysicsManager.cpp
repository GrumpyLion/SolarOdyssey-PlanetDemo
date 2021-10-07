#include "pch.h"

#include "PhysicsManager.h"

using namespace physx;

namespace PhysicsManager
{
    PxDefaultAllocator locAllocator;
    PxDefaultErrorCallback locErrorCallback;
    PxFoundation* locFoundation = nullptr;
    PxDefaultCpuDispatcher* locDispatcher = nullptr;
    PxPvd* locPvd = nullptr;

    PxPhysics* ourPhysics = nullptr;
    PxScene* ourScene = nullptr;
    PxControllerManager* ourControllerManager = nullptr;
    PxCooking* ourCooking = false;

    bool ourStopPhysics = false;
    Vec3 ourGravity = Vec3(0.0f, -9.81f, 0.0f);
}

void PhysicsManager::Initialize()
{
    locFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, locAllocator, locErrorCallback);
    locPvd = PxCreatePvd(*locFoundation);
    ourPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *locFoundation, PxTolerancesScale(), true, locPvd);
    ourCooking = PxCreateCooking(PX_PHYSICS_VERSION, *locFoundation, PxCookingParams(PxTolerancesScale()));

    PxSceneDesc sceneDesc(ourPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(ourGravity.x, ourGravity.y, ourGravity.z);
    locDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.cpuDispatcher = locDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    ourScene = ourPhysics->createScene(sceneDesc);

    PxPvdSceneClient* pvdClient = ourScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    ourControllerManager = PxCreateControllerManager(*ourScene);
}

void PhysicsManager::Shutdown()
{
    ourCooking->release();
    ourControllerManager->release();
    ourScene->release();
    locDispatcher->release();
    ourPhysics->release();
    PxPvdTransport* transport = locPvd->getTransport();
    locPvd->release();
    locFoundation->release();
}

void PhysicsManager::Update(float delta)
{
    ourScene->simulate(delta);
    ourScene->fetchResults(true);
}

RayCastHit PhysicsManager::RayCast(const Vec3& origin, const Vec3& unitDir, const float maxDistance)
{
    PxVec3 pxOrigin = PxVec3(origin.x, origin.y, origin.z);
    PxVec3 pxDir = PxVec3(unitDir.x, unitDir.y, unitDir.z);
    PxReal pxMaxDistance = maxDistance;
    PxRaycastBuffer buffer;
    RayCastHit result;

    if (ourScene->raycast(pxOrigin, pxDir, pxMaxDistance, buffer))
    {
        PxRaycastHit raycastHit = buffer.block;
        result.myHit = true;
        result.myNormal = Vec3(raycastHit.normal.x, raycastHit.normal.y, raycastHit.normal.z);
        result.myPosition = Vec3(raycastHit.position.x, raycastHit.position.y, raycastHit.position.z);
    }

    return result;
}
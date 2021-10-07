#include "pch.h"

#include "PhysicsManager.h"
#include "Rigidbody.h"

using namespace physx;

RigidbodyDynamic::RigidbodyDynamic(float mass, const PxGeometry& geometry, Vec3 position, Quat rotation)
{
    myPhysicsMaterial = PhysicsManager::ourPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    PxTransform transform = PxTransform(PxVec3(position.x, position.y, position.z));
    myRigidbody = PxCreateDynamic(*PhysicsManager::ourPhysics, transform, geometry, *myPhysicsMaterial, mass);
    PhysicsManager::ourScene->addActor(*myRigidbody);
}

RigidbodyDynamic::RigidbodyDynamic(const physx::PxGeometry& geometry, Vec3 position /*= Vec3(0.0f)*/, Quat rotation /*= Quat()*/)
{
    myPhysicsMaterial = PhysicsManager::ourPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    PxTransform transform = PxTransform(PxVec3(position.x, position.y, position.z));
    myRigidbody = PxCreateDynamic(*PhysicsManager::ourPhysics, transform, geometry, *myPhysicsMaterial, 1.0f);
    PhysicsManager::ourScene->addActor(*myRigidbody);

    //myRigidbody->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    myRigidbody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
}

RigidbodyDynamic::~RigidbodyDynamic()
{
    myRigidbody->release();
    myPhysicsMaterial->release();
}

void RigidbodyDynamic::SetGravity(Vec3 gravity)
{
}

void RigidbodyDynamic::SetVelocity(Vec3 velocity)
{
}

void RigidbodyDynamic::ApplyForce(Vec3 force)
{
}

void RigidbodyDynamic::SetKinematic(bool kinematic)
{
    myRigidbody->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
}

Vec3 RigidbodyDynamic::GetPosition()
{
    PxShape* shape = nullptr;
    myRigidbody->getShapes(&shape, 1);

    PxTransform rbt = physx::PxShapeExt::getGlobalPose(*shape, *myRigidbody);
    return Vec3(rbt.p.x, rbt.p.y, rbt.p.z);
}

Quat RigidbodyDynamic::GetRotation()
{
    PxTransform transform = myRigidbody->getGlobalPose();
    return Quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
}

Vec3 RigidbodyDynamic::GetVelocity()
{
    return {};
}


//////////////////////////////////////////////////////////////////////////


RigidbodyStatic::RigidbodyStatic(const physx::PxGeometry& geometry, Vec3 position /*= Vec3(0.0f)*/, Quat rotation /*= Quat()*/)
{
    myPhysicsMaterial = PhysicsManager::ourPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    PxTransform transform = PxTransform(PxVec3(position.x, position.y, position.z));
    myRigidbody = PxCreateStatic(*PhysicsManager::ourPhysics, transform, geometry, *myPhysicsMaterial);
    PhysicsManager::ourScene->addActor(*myRigidbody);
}

RigidbodyStatic::~RigidbodyStatic()
{
    myRigidbody->release();
    myPhysicsMaterial->release();
}

Vec3 RigidbodyStatic::GetPosition()
{
    PxShape* shape = nullptr;
    myRigidbody->getShapes(&shape, 1);

    PxTransform rbt = physx::PxShapeExt::getGlobalPose(*shape, *myRigidbody);
    return Vec3(rbt.p.x, rbt.p.y, rbt.p.z);
}
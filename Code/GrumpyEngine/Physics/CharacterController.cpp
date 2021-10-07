#include "pch.h"

#include "PhysicsManager.h"
#include "CharacterController.h"

using namespace physx;

CharacterController::CharacterController(PxControllerDesc& description)
{
    description.material = PhysicsManager::ourPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    myController = PhysicsManager::ourControllerManager->createController(description);
}

CharacterController::~CharacterController()
{
    myController->release();
}

void CharacterController::Update(float delta)
{
    PxControllerState cctState;
    myController->getState(cctState);

    myIsGrounded = false;
    if (cctState.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN)
    {
        myIsGrounded = true;
    }

    if (cctState.collisionFlags & PxControllerCollisionFlag::eCOLLISION_UP && !myIsCeiled)
    {
        myVelocity.y = 0.0f;
    }

    myIsCeiled = false;
    if (cctState.collisionFlags & PxControllerCollisionFlag::eCOLLISION_UP)
    {
        myIsCeiled = true;
    }

    if (myIsGrounded)
        myFallingVelocity = Vec3(0.0f);
    else
        myFallingVelocity += myGravity * delta;

    myVelocity += myFallingVelocity;

    PxVec3 disp = PxVec3(myVelocity.x, myVelocity.y, myVelocity.z);
    PxControllerFilters filters;

    myController->move(disp, 0.001f, delta, filters);
}

void CharacterController::SetGravity(const Vec3& gravity)
{
    myGravity = gravity;
}

void CharacterController::SetUp(const Vec3& up)
{
    myController->setUpDirection(PxVec3(up.x, up.y, up.z));
}

void CharacterController::SetVelocity(const Vec3& dir)
{
    myVelocity = dir;
}

void CharacterController::Jump(const Vec3& dir)
{
}

Vec3 CharacterController::GetPosition() const
{
    auto pos = myController->getPosition();
    return Vec3(pos.x, pos.y, pos.z);
}

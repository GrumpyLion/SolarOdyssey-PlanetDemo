#include "pch.h"

#include "Physics/PhysicsManager.h"
#include "Graphics/CameraMan.h"
#include "Input/InputMapper.h"
#include "Input/Mouse.h"
#include "Player.h"
#include "Common/Settings.h"

#if !GE_FINAL
#include <imgui.h>
#include "Editor/Editor.h"
#include "Editor/TopMenuBar.h"
#endif

using namespace physx;

Player::Player()
{
    PxCapsuleControllerDesc description;
    description.height = 1.0;
    description.radius = 0.25;
    description.position = PxExtendedVec3(10, 210, 0);

    myController = MakeUnique<CharacterController>(description);
    myCameraData.myNear = 0.25f;
    myCameraData.myFar = 500.0f; 
    myCameraData.myFOV = 90.0f;
    myCameraData.myUseGivenMatrix = true;

    AudioManager::GetSound("Data/SolarOdyssey/Sounds/Ground/Metal.ogg", myStepSound);

#if !GE_FINAL
    myRenderDebug = Settings::GetValue("PlayerDebug", false);
    myCameraActive = Settings::GetValue("CameraActive", false);

    TopMenuBar::ourMenus["Game"].myItems["Switch Active Camera"].myOnKlicked.Subscribe(this, [&](bool active) { myCameraActive = active; });
    TopMenuBar::ourMenus["Game"].myItems["Switch Active Camera"].myActive = myCameraActive;

    TopMenuBar::ourMenus["Game"].myItems["Player Editor"].myOnKlicked.Subscribe(this, [&](bool active) { myRenderDebug = active; });
    TopMenuBar::ourMenus["Game"].myItems["Player Editor"].myActive = myRenderDebug;
#endif
}

Player::~Player()
{
#if !GE_FINAL
    Settings::SetValue("PlayerDebug", myRenderDebug);
    Settings::SetValue("CameraActive", myCameraActive);
#endif
}

void Player::Update(float delta)
{
    if (InputMapper::GetBool("esc"))
        Mouse::SetMouseMode(true);

#if !GE_FINAL
    ImGuiIO& io = ImGui::GetIO();
    if (!io.WantCaptureMouse)
#endif
    {
        if (InputMapper::GetBool("shooting"))
            Mouse::SetMouseMode(false);
    }

    CalculateGravity(delta);
    myGravityDir = Math::Normalize(myGravity);
    myController->Update(delta);
    myController->SetUp(-myGravityDir);
}

void Player::Render(float delta)
{
    myController->SetVelocity(myVelocity);
    myVelocity = Vec3(0.0f);

#if !GE_FINAL
    if (myRenderDebug)
        DebugDraw();

    CameraMan::ourUseEditorCamera = !myCameraActive;

    if (myCameraActive)
#endif
        ProcessInput(delta);

    if (myController->myIsGrounded)
        myGravityVelocity = Vec3(0.0f);
    else
        myGravityVelocity += myGravity;

    myHeadBob = Math::Damp(myHeadBob, Vec3(0.0f), 5.0f, delta);
    CalculateSphericalMovement(delta);
}

#if !GE_FINAL
void Player::DebugDraw()
{
    if (!ImGui::Begin("Player Editor", &myRenderDebug))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Pos %f %f %f", myPosition.x, myPosition.y, myPosition.z);
    ImGui::Text("Vel %f %f %f", myVelocity.x, myVelocity.y, myVelocity.z);
    ImGui::Text("Gravity %f %f %f", myGravity.x, myGravity.y, myGravity.z);
    ImGui::Text("Gravity %f %f %f", myGravityVelocity.x, myGravityVelocity.y, myGravityVelocity.z);
    ImGui::Text("Forward %f %f %f", myPlanetAlignedForward.x, myPlanetAlignedForward.y, myPlanetAlignedForward.z);
    ImGui::Text("Sideway %f %f %f", mySideways.x, mySideways.y, mySideways.z);

    ImGui::DragFloat("Movement Speed", &myMovementSpeed);
    ImGui::DragFloat("Sprint Speed", &mySprintSpeed);
    ImGui::DragFloat("Mouse Speed", &myMouseSpeed);
    ImGui::DragFloat("Jump Force", &myJumpForce);

    ImGui::End();
}
#endif

void Player::CalculateSphericalMovement(float delta)
{
    myVelocity += myGravityVelocity;
    myController->SetGravity(myGravity);
    myPosition = Math::Damp(myPosition, myController->GetPosition() - myGravityDir, 50.0f, delta);

    const Vec3 rotation = myRotation + myHeadBob;
    Vec3 up = Math::Normalize(myPosition);
    myPoleDirection = Math::Normalize(myPoleDirection - Math::Dot(up, myPoleDirection) * up);
    Vec3 east = Math::Cross(myPoleDirection, up);
    Vec3 north = Math::Cross(up, east);

    Mat4 localTransform = Mat4
    (
        east.x, east.y, east.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        -north.x, -north.y, -north.z, 0.0f,
        myPosition.x, myPosition.y, myPosition.z, 1.0f
    );

    Quat localRotation = Math::AngleAxis(Math::ToRadians(rotation.y), Vec3(0.0f, 1.0f, 0.0f));
    localRotation = Math::Normalize(localRotation);

    myCameraData.myViewMatrix = localTransform * Math::Mat4Cast(localRotation);
    myPlanetAlignedForward = Math::Normalize(-1.0f * Vec3(myCameraData.myViewMatrix[2]));

    localRotation = Math::Rotate(localRotation, -Math::ToRadians(rotation.x), Vec3(1.0f, 0.0f, 0.0f));
    localRotation = Math::Normalize(localRotation);
    localRotation = Math::Rotate(localRotation, Math::ToRadians(rotation.z), Vec3(0.0f, 0.0f, 1.0f));
    localRotation = Math::Normalize(localRotation);

    myCameraData.myViewMatrix = localTransform * Math::Mat4Cast(localRotation);
    mySideways = Vec3(myCameraData.myViewMatrix[0]);
    myCameraData.myViewMatrix = Math::Inverse(myCameraData.myViewMatrix);

    CameraMan::SetCameraData(0, myCameraData);
}


void Player::CalculateGravity(float delta)
{
    const Vec3 planetPos = Vec3(0.0f);
    const float mass = 50.0f;
    const float planetMass = 30000.0f;

    Vec3 direction = Math::Normalize(planetPos - myController->GetPosition());
    float distance = Math::Distance(myController->GetPosition(), planetPos);
    float distanceSqrt = distance * distance;
    Vec3 force = direction * 6.674f * mass * planetMass / distanceSqrt;
    Vec3 acceleration = force / mass;

    myGravity = acceleration * delta;
}

void Player::ProcessInput(float delta)
{
    float speed = myMovementSpeed;

    if (InputMapper::GetBool("sprint"))
        speed = mySprintSpeed;

    const float leftAxisVertical = InputMapper::GetFloat("left axis vertical");
    const float leftAxisHorizontal = InputMapper::GetFloat("left axis horizontal");
    const float rightAxisVertical = InputMapper::GetFloat("right axis vertical");
    const float rightAxisHorizontal = InputMapper::GetFloat("right axis horizontal");

    if (!Mouse::ourMouseMode)
    {
        myVelocity += leftAxisVertical * myPlanetAlignedForward * speed * delta;
        myVelocity -= leftAxisHorizontal * -mySideways * speed * delta;
        myRotation += -Vec3(rightAxisVertical * myMouseSpeed * delta, -rightAxisHorizontal * myMouseSpeed * delta, 0);
        myHeadBob.x += leftAxisVertical * myHeadBobStrenght * delta;
        myHeadBob.z -= leftAxisHorizontal * myHeadBobStrenght * delta;
    }

    float velocity = Math::Length(myVelocity * 0.75f) * 10.f;
    if (Math::Length(myVelocity) > 0.001f && Clock::GetCurrentTimeInSec() > myLastTimeStepSoundPlayed + (0.75f - velocity))
    {
        AudioManager::Play(myStepSound);
        AudioManager::SetVolume(myStepSound, 0.5f);
        AudioManager::SetPitch(myStepSound, 1.0f + Math::RandomFloat(-0.25f, 0.25f));
        myLastTimeStepSoundPlayed = Clock::GetCurrentTimeInSec();
    }

    if (myRotation.x > 90)
        myRotation.x = 90;

    if (myRotation.x < -90)
        myRotation.x = -90;
}
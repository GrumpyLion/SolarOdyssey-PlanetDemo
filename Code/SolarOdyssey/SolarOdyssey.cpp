#include "pch.h"

#include "Common/Window.h"
#include "Common/Settings.h"
#include "Audio/AudioManager.h"
#include "Graphics/CameraMan.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/MaterialLibrary.h"
#include "Graphics/Renderer.h"
#include "Graphics/FrameData.h"
#include "Editor/Editor.h"
#include "Editor/TopMenuBar.h"
#include "Input/Keyboard.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/InputMapper.h"
#include "Physics/PhysicsManager.h"

#include "Rendering/SolarOdysseyRenderer.h"
#include "Physics/CollisionShapes.h"

#include "SolarSystem/Planet.h"
#include "Player/Player.h"
#include "Audio/AudioManager.h"

#include <chrono>

#include "SolarOdyssey.h"

#if !GE_FINAL
#include "imgui.h"
#endif

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds TIMESTEP(16ms);

namespace SolarOdyssey
{
    SPtr<SolarOdysseyRenderer> ourRenderer;
    SPtr<Planet> ourPlanet;
    SPtr<Player> ourPlayer;
    SPtr<RigidbodyDynamic> locBoxRigidbody;
    SPtr<Mesh> locBoxMesh;
    SPtr<Mesh> locBoatMesh;

    bool locIsRunning;
    std::chrono::high_resolution_clock::time_point locTimeStartTick;
    std::chrono::high_resolution_clock::time_point locTimeStart;
}

void SolarOdyssey::Initialize()
{
    srand(static_cast <unsigned> (time(0)));

    Settings::LoadSettings();

    InputMapper::Initialize();
    AudioManager::Initialize();
    PhysicsManager::Initialize();

#if !GE_FINAL
    Editor::Initialize();
#endif

    Window::Initialize();
    Window::SetTitle("Solar Odyssey");

    Keyboard::Initialize();
    Mouse::Initialize();

    Renderer::Initialize();
    MaterialLibrary::LoadFromJSON("Data/SolarOdyssey/Materials.json");

    ourRenderer = MakeShared<SolarOdysseyRenderer>();
    Renderer::ourCustomRenderer = ourRenderer;

    Window::ourCloseEvent.Subscribe(SolarOdyssey::Initialize, [&]() { locIsRunning = false; });

    //////////////////////////////////////////////////////////////////////////

    locBoxMesh = Mesh::Create("Data/GrumpyEngine/Models/Cube.glb");
    locBoatMesh = Mesh::Create("Data/SolarOdyssey/Models/Boat.glb");

    ourPlayer = MakeShared<Player>();
    ourPlanet = MakeShared<Planet>();
    locBoxRigidbody = MakeShared<RigidbodyDynamic>(10.0f, BoxShape(0.5, 0.5, 0.5), Vec3(0, 60, 0));

    Run();
}

void SolarOdyssey::Shutdown()
{
    locBoxRigidbody.reset();
    ourPlanet.reset();
    ourPlayer.reset();

#if GE_PLATFORM_PC
    glfwSetWindowShouldClose(Window::ourWindowHandle, 1);
    Window::Hide();

    Window::Shutdown();
#endif
    AudioManager::Shutdown();

#if !GE_FINAL
    Editor::Shutdown();
#endif

    PhysicsManager::Shutdown();
    Renderer::Shutdown();
    Settings::SaveSettings();
}

void SolarOdyssey::Run()
{
    using clock = std::chrono::high_resolution_clock;

    std::chrono::nanoseconds lag(0ns);
    locTimeStart = clock::now();
    locTimeStartTick = clock::now();
    auto timeStartFPS = clock::now();
    int internalFPS = 0;

    Window::Show();

    locIsRunning = true;
    while (locIsRunning)
    {
        std::chrono::duration<float> deltaTimeFrame = clock::now() - locTimeStart;
        locTimeStart = clock::now();
        lag += std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTimeFrame);

        Window::Update();
        Renderer::PreRender();

        Update(deltaTimeFrame.count());
        AudioManager::Update();

#if !GE_FINAL
        Editor::Render(deltaTimeFrame.count());
#endif

        PhysicsManager::Update(deltaTimeFrame.count());

        Render(deltaTimeFrame.count());
        Renderer::Render(deltaTimeFrame.count());
        Renderer::PostRender();

        internalFPS++;
        std::chrono::duration<float> deltaFPSTime = clock::now() - timeStartFPS;
        if (deltaFPSTime.count() > 1)
        {
            internalFPS = 0;
            timeStartFPS = clock::now();
        }

        InputMapper::Update();

        Keyboard::Update();
        Mouse::Update();
    }
}

void SolarOdyssey::Update(float delta)
{
    ourPlayer->Update(delta);
}

void SolarOdyssey::Render(float delta)
{
    Renderer::ourFrameData.AddInstance(locBoxMesh, Material::Create("Default"), Math::Translate(locBoxRigidbody->GetPosition()) * Math::Rotate(locBoxRigidbody->GetRotation()));
    Renderer::ourFrameData.AddInstance(locBoatMesh, Material::Create("Boat"), Math::Translate(Vec3(-40.9f, 11.0f, -26.0f)) * Math::Rotate(Vec3(5.0f, -30.0f, 60.0f)) * locBoatMesh->myLocalMatrix);

    ourPlanet->Render(delta);
    ourPlayer->Render(delta);
}
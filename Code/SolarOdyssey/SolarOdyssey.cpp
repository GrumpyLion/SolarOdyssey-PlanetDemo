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

#include "SolarOdyssey.h"

#if !GE_FINAL
#include "imgui.h"
#endif

using namespace std::chrono_literals;
constexpr std::chrono::nanoseconds TIMESTEP(16ms);

SolarOdyssey::SolarOdyssey()
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
    Renderer::ourCustomRenderer = MakeShared<SolarOdysseyRenderer>();

    Window::ourCloseEvent.Subscribe(this, [&]() { myIsRunning = false; });

    //////////////////////////////////////////////////////////////////////////

    AudioManager::GetSound("Data/SolarOdyssey/Sounds/Ambient.ogg", myAmbientSound);
    AudioManager::Play(myAmbientSound);
    AudioManager::SetLooping(myAmbientSound, true);

    myPlayer = MakeShared<Player>();
    myPlanet = MakeShared<Planet>();
    myBoxRigidbody = MakeShared<RigidbodyDynamic>(10.0f, BoxShape(0.5, 0.5, 0.5), Vec3(0, 210, 0));

    Run();
}

SolarOdyssey::~SolarOdyssey()
{
    myBoxRigidbody.reset();
    myPlanet.reset();
    myPlayer.reset();

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
    myTimeStart = clock::now();
    myTimeStartTick = clock::now();
    auto timeStartFPS = clock::now();
    int internalFPS = 0;

    Window::Show();

    myIsRunning = true;
    while (myIsRunning)
    {
        std::chrono::duration<float> deltaTimeFrame = clock::now() - myTimeStart;
        myTimeStart = clock::now();
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
    myPlayer->Update(delta);
}

void SolarOdyssey::Render(float delta)
{
    Renderer::ourFrameData.AddInstance(Mesh::Create("Data/GrumpyEngine/Models/Cube.glb"), Material::Create("Default"), Math::Translate(myBoxRigidbody->GetPosition()) * Math::Rotate(myBoxRigidbody->GetRotation()));
    Renderer::ourFrameData.AddInstance(Mesh::Create("Data/SolarOdyssey/Models/Nature/DeadTree.glb"), Material::Create("DeadTree"), Math::Translate(Vec3(-199.9f, -5, 0)) * Math::Rotate(Vec3(0.0f, 0.0f, 90.0f)));

    myPlanet->Render(delta);
    myPlayer->Render(delta);
}
#pragma once

#include "SolarSystem/Planet.h"
#include "Player/Player.h"
#include "Audio/AudioManager.h"

#include <chrono>

struct SolarOdyssey
{
    SolarOdyssey();
    ~SolarOdyssey();

    void Run();

    void Update(float delta);
    void Render(float delta);

    SPtr<Planet> myPlanet;
    SPtr<Player> myPlayer;
    SPtr<RigidbodyDynamic> myBoxRigidbody;
    AudioHandle myAmbientSound;

    bool myIsRunning;
    std::chrono::high_resolution_clock::time_point myTimeStartTick;
    std::chrono::high_resolution_clock::time_point myTimeStart;
};
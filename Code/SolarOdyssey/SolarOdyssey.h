#pragma once

struct Planet;
struct Player;
struct SolarOdysseyRenderer;

namespace SolarOdyssey
{
    void Initialize();
    void Shutdown();

    void Run();

    void Update(float delta);
    void Render(float delta);

    extern SPtr<SolarOdysseyRenderer> ourRenderer;
    extern SPtr<Planet> ourPlanet;
    extern SPtr<Player> ourPlayer;
};
#include "pch.h"

#if GE_PLATFORM_PC
#include "Settings.h"
#include <fstream>

namespace Settings
{
    nlohmann::json ourNode{};
}

void Settings::LoadSettings()
{
    std::ifstream file("Settings.json");
    if (file.is_open())
        file >> ourNode;
}

void Settings::SaveSettings()
{
    std::ofstream fout("Settings.json");
    fout << ourNode;
}
#endif
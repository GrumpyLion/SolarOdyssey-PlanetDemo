#pragma once

namespace InputMapper
{
    void Initialize();
    void Update();

    float GetFloat(const std::string& inputName);
    bool GetBool(const std::string& inputName);
}
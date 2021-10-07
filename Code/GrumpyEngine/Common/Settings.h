#pragma once

#include <nlohmann/json.hpp>

namespace Settings
{
    void LoadSettings();
    void SaveSettings();

    extern nlohmann::json ourNode;

    template<typename T>
    inline T GetValue(const std::string& name, const T& value = T{})
    {
        if (!ourNode[name].is_null())
            return ourNode[name].get<T>();

        ourNode[name] = value;
        return ourNode[name].get<T>();
    }

    template<typename T>
    inline void SetValue(const std::string& name, const T& value)
    {
        ourNode[name] = value;
    }
}
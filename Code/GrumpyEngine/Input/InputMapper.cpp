#include "pch.h"

#include "Keyboard.h"
#include "Mouse.h"
#include "Window.h"

#include "InputMapper.h"

namespace InputMapper
{
    std::unordered_map<std::string, float> locInputValues;
}

void InputMapper::Initialize()
{
    locInputValues.insert({ "left axis vertical", 0.0f });
    locInputValues.insert({ "left axis horizontal", 0.0f });
    locInputValues.insert({ "right axis vertical", 0.0f });
    locInputValues.insert({ "right axis horizontal", 0.0f });
    locInputValues.insert({ "shooting", 0.0f });
    locInputValues.insert({ "left mouse just clicked", 0.0f });
    locInputValues.insert({ "jump", 0.0f });
    locInputValues.insert({ "esc", 0.0f });
    locInputValues.insert({ "tab", 0.0f });
    locInputValues.insert({ "sprint", 0.0f });

    locInputValues.insert({ "mouseX", 0.0f });
    locInputValues.insert({ "mouseY", 0.0f });
}

void InputMapper::Update()
{
    if (Keyboard::IsKeyDown(KEY_W))
        locInputValues["left axis vertical"] = 1.0f;
    else if (Keyboard::IsKeyDown(KEY_S))
        locInputValues["left axis vertical"] = -1.0f;
    else
        locInputValues["left axis vertical"] = 0.0f;

    if (Keyboard::IsKeyDown(KEY_A))
        locInputValues["left axis horizontal"] = -1.0f;
    else if (Keyboard::IsKeyDown(KEY_D))
        locInputValues["left axis horizontal"] = 1.0f;
    else
        locInputValues["left axis horizontal"] = 0.0f;

    locInputValues["right axis horizontal"] = Mouse::ourXDeltaPos;
    locInputValues["right axis vertical"] = Mouse::ourYDeltaPos;

    locInputValues["shooting"] = Mouse::IsButtonDown(MOUSE_BUTTON_LEFT) ? 1.0f : 0.0f;
    locInputValues["left mouse just clicked"] = Mouse::IsButtonJustDown(MOUSE_BUTTON_LEFT) ? 1.0f : 0.0f;
    locInputValues["jump"] = Keyboard::IsKeyDown(KEY_SPACE) ? 1.0f : 0.0f;
    locInputValues["esc"] = Keyboard::IsKeyDown(KEY_ESCAPE) ? 1.0f : 0.0f;
    locInputValues["tab"] = Keyboard::IsKeyDown(KEY_TAB) ? 1.0f : 0.0f;
    locInputValues["sprint"] = Keyboard::IsKeyDown(KEY_LEFT_SHIFT) ? 1.0f : 0.0f;

    locInputValues["mouseX"] = (float)Mouse::ourXPos;
    locInputValues["mouseY"] = Window::ourWindowDimensions.y - (float)Mouse::ourYPos;
}

float InputMapper::GetFloat(const std::string& inputName)
{
    if (locInputValues.find(inputName) == locInputValues.end())
        return 0.0f;

    return locInputValues.find(inputName)->second;
}

bool InputMapper::GetBool(const std::string& inputName)
{
    return Math::Abs(GetFloat(inputName)) > 0.0f;
}
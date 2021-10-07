#include "pch.h"

#if GE_PLATFORM_PC

#if !GE_FINAL
#include "imgui.h"
#include "imgui_impl_glfw.h"
#endif

#include "Mouse.h"
#include "Window.h"

namespace Mouse
{
    bool locButtons[MAX_BUTTONS];
    bool locButtonsClicked[MAX_BUTTONS];

    float locXOldPos = 0.0f;
    float locYOldPos = 0.0f;

    float ourXPos = 0.0f;
    float ourYPos = 0.0f;

    float ourXDeltaPos = 0.0f;
    float ourYDeltaPos = 0.0f;

    float ourScroll = 0.0f;
    bool ourMouseMode = false;
    bool ourDisableMouse = false;

    Event<void(uint)> ourButtonDownEvent;
    Event<void(uint)> ourButtonUpEvent;
    Event<void(float, float)> ourPositionEvent;
    Event<void(float)> ourScrollEvent;
}

void Mouse::Initialize()
{
    glfwSetMouseButtonCallback(Window::ourWindowHandle, [](GLFWwindow* window, int button, int action, int mods)
    {
#if !GE_FINAL
        ImGuiIO& io = ImGui::GetIO();
        if (ourDisableMouse)
        {
            memset(locButtons, 0, sizeof(locButtons));
            return;
        }

        if (ourMouseMode)
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
#endif

            if (button >= 0 && button < MAX_BUTTONS)
            {
                if (action == GLFW_PRESS)
                {
                    ourButtonDownEvent.Fire(button);

                    if (button >= 0 && button < MAX_BUTTONS)
                    {
                        locButtons[button] = true;
                    }
                }
                else
                {
                    ourButtonUpEvent.Fire(button);

                    if (button >= 0 && button < MAX_BUTTONS)
                    {
                        locButtons[button] = false;
                        locButtonsClicked[button] = false;
                    }
                }
            }
    });

    glfwSetCursorPosCallback(Window::ourWindowHandle, [](GLFWwindow* window, double x, double y)
    {
        ourXPos = (float)x;
        ourYPos = (float)y;
        ourPositionEvent.Fire((float)x, (float)y);
    });

    glfwSetScrollCallback(Window::ourWindowHandle, [](GLFWwindow* window, double x, double y)
    {
#if !GE_FINAL
        ImGui_ImplGlfw_ScrollCallback(window, x, y);
#endif

        ourScroll = (float)y;
        ourScrollEvent.Fire((float)y);
    });

    SetMouseMode(true);
}

void Mouse::Update()
{
    for (int i = 0; i < MAX_BUTTONS; ++i)
    {
        if (locButtons[i])
            locButtonsClicked[i] = true;
    }

    ourScroll = 0;
    ourXDeltaPos = (locXOldPos - ourXPos);
    ourYDeltaPos = (locYOldPos - ourYPos);

    locXOldPos = ourXPos;
    locYOldPos = ourYPos;
}

bool Mouse::IsButtonDown(short index)
{
    if (index < 0 && index > MAX_BUTTONS)
    {
        return false;
    }
    return locButtons[index];
}

bool Mouse::IsButtonJustDown(short index)
{
    if (index < 0 && index > MAX_BUTTONS)
    {
        return false;
    }
    return !locButtonsClicked[index] && locButtons[index];
}

void Mouse::SetMouseMode(bool enabled)
{
    if (ourMouseMode == enabled)
        return;

    locXOldPos = ourXPos;
    locYOldPos = ourYPos;

    ourMouseMode = enabled;
    glfwSetInputMode(Window::ourWindowHandle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

#endif // GE_PLATFORM_PC
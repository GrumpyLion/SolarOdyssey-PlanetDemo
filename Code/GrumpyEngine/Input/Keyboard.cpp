#include "pch.h"

#if GE_PLATFORM_PC
#include "Keyboard.h"
#include "Window.h"

#if !GE_FINAL
#include "imgui.h"
#include "imgui_impl_glfw.h"
#endif

namespace Keyboard
{
    bool locKeys[MAX_KEYS];
    bool locKeysClicked[MAX_KEYS];

    bool ourDisableKeyboard = false;
    Event<void(uint)> ourKeyDownEvent;
    Event<void(uint)> ourKeyUpEvent;
    Event<void(uint)> ourKeyTypedEvent;
}

void Keyboard::Initialize()
{
    glfwSetKeyCallback(Window::ourWindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
#if !GE_FINAL
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        if (ourDisableKeyboard)
        {
            memset(&locKeys, 0, sizeof(locKeys));
            return;
        }
#endif

        switch (action)
        {
        case GLFW_PRESS:
        {
            if (key >= 0 && key < MAX_KEYS)
            {
                locKeys[key] = true;
            }

            Keyboard::ourKeyDownEvent.Fire(key);
            break;
        }
        case GLFW_RELEASE:
        {
            if (key >= 0 && key < MAX_KEYS)
            {
                locKeysClicked[key] = false;
                locKeys[key] = false;
            }

            Keyboard::ourKeyUpEvent.Fire(key);
            break;
        }
        case GLFW_REPEAT:
        {
            Keyboard::ourKeyDownEvent.Fire(key);
            break;
        }
        }
    });

    glfwSetCharCallback(Window::ourWindowHandle, [](GLFWwindow* window, uint codepoint)
    {
#if !GE_FINAL
        ImGui_ImplGlfw_CharCallback(window, codepoint);
#endif

        Keyboard::ourKeyTypedEvent.Fire(codepoint);
    });

    for (int i = 0; i < MAX_KEYS; ++i)
    {
        locKeys[i] = 0;
    }
}

bool Keyboard::IsKeyDown(short index)
{
    if (index < 0 && index >= MAX_KEYS)
    {
        return false;
    }
    return locKeys[index];
}

bool Keyboard::IsKeyJustDown(short index)
{
    if (index < 0 && index >= MAX_KEYS)
    {
        return false;
    }
    return !locKeysClicked[index] && locKeys[index];
}

void Keyboard::Update()
{
    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (locKeys[i])
            locKeysClicked[i] = true;
    }
}

#endif // GE_PLATFORM_PC
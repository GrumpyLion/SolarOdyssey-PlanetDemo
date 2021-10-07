#pragma once

#if GE_PLATFORM_PC
#include <windows.h>
#include <glfw/glfw3.h>

namespace Window
{
    void Initialize();
    void Update();
    void Shutdown();

    void Show();
    void Hide();

    void SetPosition(int x, int y);
    void SetDimensions(int width, int height, bool center = true);
    void SetTitle(const std::string& name);
    void CenterWindow();

    void LoadSettings();
    void SaveSettings();

    //////////////////////////////////////////////////////////////////////////

    extern Event<void(int, int)> ourResizeEvent;
    extern Event<void(int, int)> ourPositionEvent;
    extern Event<void()> ourCloseEvent;

    extern GLFWwindow* ourWindowHandle;

    extern bool ourBorderless;
    extern std::string ourTitle;

    extern Vec2i ourWindowDimensions;
    extern Vec2i ourScreenDimensions;
    extern Vec2i ourWindowPos;
};
#endif // GE_PLATFORM_PC
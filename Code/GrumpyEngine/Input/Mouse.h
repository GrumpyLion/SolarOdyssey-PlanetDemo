#pragma once

#if GE_PLATFORM_PC

#define MOUSE_BUTTON_1         0
#define MOUSE_BUTTON_2         1
#define MOUSE_BUTTON_3         2
#define MOUSE_BUTTON_4         3
#define MOUSE_BUTTON_5         4
#define MOUSE_BUTTON_6         5
#define MOUSE_BUTTON_7         6
#define MOUSE_BUTTON_8         7
#define MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
#define MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
#define MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
#define MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

constexpr int MAX_BUTTONS = 16;

namespace Mouse
{
    void Initialize();
    void Update();

    bool IsButtonDown(short index);
    bool IsButtonJustDown(short index);
    void SetMouseMode(bool enabled);

    //////////////////////////////////////////////////////////////////////////

    extern float ourXPos;
    extern float ourYPos;

    extern float ourXDeltaPos;
    extern float ourYDeltaPos;

    extern float ourScroll;
    extern bool ourMouseMode;
    extern bool ourDisableMouse;

    extern Event<void(uint)> ourButtonDownEvent;
    extern Event<void(uint)> ourButtonUpEvent;
    extern Event<void(float, float)> ourPositionEvent;
    extern Event<void(float)> ourScrollEvent;
};

#endif
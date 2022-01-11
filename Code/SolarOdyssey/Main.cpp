#include "pch.h"

#include <crtdbg.h>

#include "Window.h"
#include "SolarOdyssey.h"

#if GE_PLATFORM_PC
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
#if GE_PLATFORM_PC && !GE_FINAL
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    {
        SolarOdyssey::Initialize();
        SolarOdyssey::Shutdown();
    }

    return 0;
}
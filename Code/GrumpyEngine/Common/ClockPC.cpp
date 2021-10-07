#include "pch.h"

#if GE_PLATFORM_PC

#include "Clock.h"
#include <GLFW/glfw3.h>

float Clock::GetCurrentTimeInSec()
{
    return (float)glfwGetTime();
}

#endif
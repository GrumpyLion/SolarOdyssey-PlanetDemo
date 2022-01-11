#include "pch.h"

#if GE_PLATFORM_PC
#include "Window.h"
#include "Settings.h"

namespace Window
{
    Event<void(int, int)> ourResizeEvent;
    Event<void(int, int)> ourPositionEvent;
    Event<void()> ourCloseEvent;

    GLFWwindow* ourWindowHandle = nullptr;

    bool ourBorderless = false;
    std::string ourTitle = "Game";

    Vec2i ourWindowDimensions = Vec2i(1600, 900);
    Vec2i ourScreenDimensions = Vec2i(0, 0);
    Vec2i ourWindowPos = Vec2i(0, 0);
}

void Window::Initialize()
{
    glfwInit();

    LoadSettings();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    if (ourBorderless)
    {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

#if !GE_FINAL
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    ourWindowHandle = glfwCreateWindow(ourWindowDimensions.x, ourWindowDimensions.y, ourTitle.c_str(), nullptr, nullptr);

    glfwMakeContextCurrent(ourWindowHandle);

    glfwSetWindowPosCallback(ourWindowHandle, [](GLFWwindow* window, int x, int y)
    {
        ourWindowPos = Vec2i(x, y);
        Window::ourPositionEvent.Fire(x, y);
    });

    glfwSetWindowSizeCallback(ourWindowHandle, [](GLFWwindow* window, int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        ourWindowDimensions = Vec2i(width, height);
        Window::ourResizeEvent.Fire(width, height);
    });

    glfwSetWindowCloseCallback(ourWindowHandle, [](GLFWwindow* window)
    {
        Hide();
        Window::ourCloseEvent.Fire();
    });

    CenterWindow();
}

void Window::Shutdown()
{
    SaveSettings();
    glfwDestroyWindow(ourWindowHandle);
    glfwTerminate();
}

void Window::Update()
{
    glfwPollEvents();
}

void Window::Show()
{
    glfwShowWindow(ourWindowHandle);
    ourResizeEvent.Fire(ourWindowDimensions.x, ourWindowDimensions.y);
}

void Window::Hide()
{
    glfwHideWindow(ourWindowHandle);
}

void Window::SetPosition(int x, int y)
{
    ourWindowPos = Vec2i(x, y);

    glfwSetWindowPos(ourWindowHandle, x, y);
}

void Window::SetDimensions(int width, int height, bool center /*= true*/)
{
    ourWindowDimensions = Vec2i(width, height);
    glfwSetWindowSize(ourWindowHandle, width, height);

    if (center)
        CenterWindow();
}

void Window::SetTitle(const std::string& name)
{
    ourTitle = name;
    glfwSetWindowTitle(ourWindowHandle, ourTitle.c_str());
}

void Window::CenterWindow()
{
    int width = 0, height = 0;

    glfwGetWindowSize(ourWindowHandle, &width, &height);

    // Get the resolution of the primary monitor
    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    ourScreenDimensions = Vec2i(vidmode->width, vidmode->height);

    // Center the window
    glfwSetWindowPos(
        ourWindowHandle,
        (vidmode->width - width) / 2,
        (vidmode->height - height) / 2
    );

    int xPos = 0, yPos = 0;

    glfwGetWindowPos(ourWindowHandle, &xPos, &yPos);

    ourWindowPos = Vec2i(xPos, yPos);
}

void Window::LoadSettings()
{
    //ourWindowDimensions.x = Settings::GetValue<int>("width", 1300);
    //ourWindowDimensions.y = Settings::GetValue<int>("height", 720);

    ourBorderless = Settings::GetValue<bool>("borderless", false);
}

void Window::SaveSettings()
{
    //Settings::SetValue("width", ourWindowDimensions.x);
    //Settings::SetValue("height", ourWindowDimensions.y);

    Settings::SetValue("borderless", ourBorderless);
}
#endif // GE_PLATFORM_PC
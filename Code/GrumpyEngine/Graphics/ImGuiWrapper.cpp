#include "pch.h"

#if !GE_FINAL
#include <imgui.h>

#include "ImGuiWrapper.h"
#include "Renderer.h"
#include "Window.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "Editor/Gizmos.h"

void ImGuiWrapper::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;

#if GE_PLATFORM_PC
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
#endif

    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

    ImGui_ImplOpenGL3_Init("#version 410");
#if GE_PLATFORM_PC
    ImGui_ImplGlfw_InitForOpenGL(Window::ourWindowHandle, false);
#endif

    ImGui::StyleColorsClassic();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(5.0f, 5.0f);
    style.WindowRounding = 0.0f;
}

void ImGuiWrapper::Destroy()
{
    ImGui_ImplOpenGL3_Shutdown();
#if GE_PLATFORM_PC
    ImGui_ImplGlfw_Shutdown();
#endif
    ImGui::DestroyContext();
}

void ImGuiWrapper::PreRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    Gizmos::BeginFrame();
}

void ImGuiWrapper::Render()
{
    //ImGui::ShowDemoWindow(nullptr);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

#if GE_PLATFORM_PC
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
#endif
}

#endif
#include "pch.h"

#if !GE_FINAL
#include "imgui.h"
#include "imgui_internal.h"

#include "Gizmos.h"
#include "TextureViewer.h"
#include "TopMenuBar.h"

#include "Graphics/Renderer.h"
#include "Window.h"
#include "Editor.h"

#include "Graphics/CameraMan.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Audio/AudioManager.h"
#include "Settings.h"

namespace Editor
{
    struct Transform
    {
        Vec3 myPosition;
        Vec3 myRotation;
    };

    Transform locEditorCameraTransform;
    float locEditorCameraSpeed = 50.0f;
    Vec2 locGameViewSize;

    bool locShowTextureView = false;
    bool locShowOptionsView = false;

#if GE_PLATFORM_PC
    void RenderGuizmo()
    {
        ImGuiIO& io = ImGui::GetIO();
        Gizmos::SetOrthographic(false);
        Gizmos::SetRect((float)Window::ourWindowPos.x, (float)Window::ourWindowPos.y, io.DisplaySize.x, io.DisplaySize.y);
    }
#endif

    void RenderSimpleOverlay()
    {
        const float DISTANCE = 0.0f;
        static int corner = 3;
        if (corner != -1)
        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 window_pos = ImVec2((corner & 1) ? (viewport->Pos.x + viewport->Size.x - DISTANCE) : (viewport->Pos.x + DISTANCE), (corner & 2) ? (viewport->Pos.y + viewport->Size.y - DISTANCE) : (viewport->Pos.y + DISTANCE));
            ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        }
        ImGui::SetNextWindowBgAlpha(0.5f);
        bool open = true;
        if (ImGui::Begin("Simple overlay", &open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("Editor Cam Pos X:%f Y:%f Z:%f", locEditorCameraTransform.myPosition.x, locEditorCameraTransform.myPosition.y, locEditorCameraTransform.myPosition.z);
            ImGui::Text("Editor Cam Rot X:%f Y:%f Z:%f", locEditorCameraTransform.myRotation.x, locEditorCameraTransform.myRotation.y, locEditorCameraTransform.myRotation.z);
        }
        ImGui::End();
    }

    void UpdateEditorCamera(float delta)
    {
        float sinXRot = Math::Sin(Math::ToRadians(locEditorCameraTransform.myRotation.x));
        float cosXRot = Math::Cos(Math::ToRadians(locEditorCameraTransform.myRotation.x));

        float sinYRot = Math::Sin(Math::ToRadians(locEditorCameraTransform.myRotation.y + 90));
        float cosYRot = Math::Cos(Math::ToRadians(locEditorCameraTransform.myRotation.y + 90));

        Vec3 camForward = Math::GetForwardVector(locEditorCameraTransform.myRotation);
        Vec3 camSideways = Math::GetSidewaysVector(locEditorCameraTransform.myRotation);

        float camSpeed = locEditorCameraSpeed;
        if (Keyboard::IsKeyDown(KEY_LEFT_SHIFT))
            camSpeed *= 2;

        if (Keyboard::IsKeyDown(KEY_W))
        {
            locEditorCameraTransform.myPosition += camForward * camSpeed * delta;
        }
        if (Keyboard::IsKeyDown(KEY_A))
        {
            locEditorCameraTransform.myPosition += camSideways * camSpeed * delta;
        }
        if (Keyboard::IsKeyDown(KEY_S))
        {
            locEditorCameraTransform.myPosition -= camForward * camSpeed * delta;
        }
        if (Keyboard::IsKeyDown(KEY_D))
        {
            locEditorCameraTransform.myPosition -= camSideways * camSpeed * delta;
        }
        if (Keyboard::IsKeyDown(KEY_Q))
        {
            locEditorCameraTransform.myPosition -= Vec3(0.0f, 1.0f, 0.0f) * camSpeed * delta;
        }
        if (Keyboard::IsKeyDown(KEY_E))
        {
            locEditorCameraTransform.myPosition += Vec3(0.0f, 1.0f, 0.0f) * camSpeed * delta;
        }

        ImGuiIO& io = ImGui::GetIO();
        if (Mouse::IsButtonDown(MOUSE_BUTTON_1) && !io.WantCaptureMouse)
        {
            Mouse::SetMouseMode(false);

            locEditorCameraTransform.myRotation.x -= Mouse::ourYDeltaPos * 25.0f * delta;
            locEditorCameraTransform.myRotation.y += Mouse::ourXDeltaPos * 25.0f * delta;
        }
        else
            Mouse::SetMouseMode(true);

        //////////////////////////////////////////////////////////////////////////

        CameraMan::CameraData cameraData;
        cameraData.myRotation = locEditorCameraTransform.myRotation;
        cameraData.myPosition = locEditorCameraTransform.myPosition;
        cameraData.myFar = 500.0f;
        cameraData.myNear = 0.5f;
        cameraData.myFOV = 70.0f;
        cameraData.myInterpolate = false;
        cameraData.myUseGivenMatrix = false;
        CameraMan::ourCurrentCamera = 0;
        CameraMan::SetCameraData(63, cameraData);
        CameraMan::ourUseEditorCamera = true;

        AudioManager::SetListenerData(locEditorCameraTransform.myPosition * Vec3(-1, 1, 1),
            Vec3(0.0f), -Math::GetForwardVector(locEditorCameraTransform.myRotation), Math::GetUpVector(locEditorCameraTransform.myRotation));
    }

    void ShowEditorSettings()
    {
        ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
        if (!ImGui::Begin("Editor Settings"))
        {
            ImGui::End();
            return;
        }

        ImGui::DragFloat("Camera Speed", &locEditorCameraSpeed);

        ImGui::End();
    }
}

void Editor::Initialize()
{
    TopMenuBar::ourMenus["Editor"].myItems["Show Textures"].myOnKlicked.Subscribe(Editor::Initialize, [&](bool active) { locShowTextureView = active; });
    TopMenuBar::ourMenus["Editor"].myItems["Show Options"].myOnKlicked.Subscribe(Editor::Initialize, [&](bool active) { locShowOptionsView = active; });

    locEditorCameraSpeed = Settings::GetValue<float>("EditorCameraSpeed", 50.0f);
    CameraMan::ourUseEditorCamera = Settings::GetValue<bool>("UseEditorCamera", true);

    locEditorCameraTransform.myPosition.x = Settings::GetValue<float>("EditorCameraPosX", -180.0f);
    locEditorCameraTransform.myPosition.y = Settings::GetValue<float>("EditorCameraPosY", 0.0f);
    locEditorCameraTransform.myPosition.z = Settings::GetValue<float>("EditorCameraPosZ", -400.0f);

    locEditorCameraTransform.myRotation.x = Settings::GetValue<float>("EditorCameraRotX", 0.0f);
    locEditorCameraTransform.myRotation.y = Settings::GetValue<float>("EditorCameraRotY", 25.0f);
    locEditorCameraTransform.myRotation.z = Settings::GetValue<float>("EditorCameraRotZ", 0.0f);
}

void Editor::Shutdown()
{
    Settings::SetValue<float>("EditorCameraSpeed", locEditorCameraSpeed);
    Settings::SetValue<bool>("UseEditorCamera", CameraMan::ourUseEditorCamera);

    Settings::SetValue<float>("EditorCameraPosX", locEditorCameraTransform.myPosition.x);
    Settings::SetValue<float>("EditorCameraPosY", locEditorCameraTransform.myPosition.y);
    Settings::SetValue<float>("EditorCameraPosZ", locEditorCameraTransform.myPosition.z);

    Settings::SetValue<float>("EditorCameraRotX", locEditorCameraTransform.myRotation.x);
    Settings::SetValue<float>("EditorCameraRotY", locEditorCameraTransform.myRotation.y);
    Settings::SetValue<float>("EditorCameraRotZ", locEditorCameraTransform.myRotation.z);
}

void Editor::Render(float delta)
{
    RenderSimpleOverlay();

#if GE_PLATFORM_PC
#if GE_NETWORK_ENABLED
    if (TopMenuBar::ourShowNetworkView)
        NetworkView::Render();
#endif

    TopMenuBar::Render();
#endif

    if (locShowTextureView)
        TextureViewer::Render();

    if (locShowOptionsView)
        ShowEditorSettings();

    if (CameraMan::ourUseEditorCamera)
        UpdateEditorCamera(delta);
        


#if GE_PLATFORM_PC
    RenderGuizmo();
#endif
}
#endif
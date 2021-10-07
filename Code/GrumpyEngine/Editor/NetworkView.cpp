#include "pch.h"

#if !GE_FINAL && GE_ENABLE_NETWORKING

#include "Win_Debug_NetworkView.h"

void NetworkView::Render()
{
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
    if (!ImGui::Begin("Network"))
    {
        ImGui::End();
        return;
    }

    ImGui::Text("Client");
    static char clientIP[256];
    if (strnlen_s(clientIP, 256) == 0)
        memcpy(clientIP, "localhost", strlen("localhost"));
    ImGui::InputText("Client IP", clientIP, 256);

    if (Core_Win_NetworkManager::ourIsClient)
    {
        if (ImGui::Button("Disconnect Client"))
        {
            Core_Win_NetworkManager::Disconnect();
        }
    }
    else
    {
        if (ImGui::Button("Connect Client"))
        {
            Core_Win_NetworkManager::StopServer();
            Core_Win_NetworkManager::ourIP = clientIP;
            Core_Win_NetworkManager::Connect();
        }
    }

    ImGui::Separator();
    ImGui::Text("Server");
    static char serverIP[256];
    ImGui::InputText("Server IP", serverIP, 256);

    if (Core_Win_NetworkManager::ourIsServer)
    {
        if (ImGui::Button("Stop Server"))
        {
            Core_Win_NetworkManager::StopServer();
        }
    }
    else
    {
        if (ImGui::Button("Start Server"))
        {
            //Scene::Initialize();
            Core_Win_NetworkManager::Disconnect();
            Core_Win_NetworkManager::StartServer();
        }
    }

    static char username[256];
    if (strnlen_s(username, 256) == 0)
        memcpy(username, Core_Win_NetworkManager::ourUserName.c_str(), Core_Win_NetworkManager::ourUserName.length());
    if (ImGui::InputText("Username", username, 256, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        Core_Win_NetworkManager::ourUserName = std::string(username);
    }

    ImGui::End();
}
#endif
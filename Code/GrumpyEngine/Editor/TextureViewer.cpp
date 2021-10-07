#include "pch.h"

#if !GE_FINAL
#include "imgui.h"
#include "TextureViewer.h"
#include "Graphics/Texture.h"

void TextureViewer::Render()
{
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Once);
    if (!ImGui::Begin("Texture View"))
    {
        ImGui::End();
        return;
    }

    static float height = 350.0f;
    ImGui::DragFloat("Height", &height, 10.0f);
    auto textures = Texture::ourTextures;
    for (auto& texture : textures)
    {
        float aspect = (float)texture.second->myWidth / (float)texture.second->myHeight;
        ImGui::Image((ImTextureID)texture.second->myTextureID, ImVec2(height * aspect, height));
        ImGui::SameLine();
        ImGui::LabelText("%s", texture.first.c_str());
    }

    ImGui::End();
}

#endif
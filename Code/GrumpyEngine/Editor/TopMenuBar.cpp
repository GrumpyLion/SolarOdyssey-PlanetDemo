#include "pch.h"

#if !GE_FINAL
#include "imgui.h"
#include "TopMenuBar.h"

namespace TopMenuBar
{
    std::map<std::string, Menu> ourMenus;
}

void TopMenuBar::Render()
{
    if (ImGui::BeginMainMenuBar())
    {
        for (auto& menu : ourMenus)
        {
            if (ImGui::BeginMenu(menu.first.c_str()))
            {
                for (auto& item : menu.second.myItems)
                {
                    if (ImGui::MenuItem(item.first.c_str(), "", &item.second.myActive))
                        item.second.myOnKlicked.Fire(item.second.myActive);
                }

                ImGui::EndMenu();
            }
        }
    }
    ImGui::EndMainMenuBar();
}

#endif
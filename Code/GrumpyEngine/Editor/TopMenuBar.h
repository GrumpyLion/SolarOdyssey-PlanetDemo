#pragma once

#include <map>

#if !GE_FINAL

namespace TopMenuBar
{
    void Render();

    struct MenuItem
    {
        std::string myName = "";
        bool myActive = false;
        Event<void(bool)> myOnKlicked;
    };

    struct Menu
    {
        std::map<std::string, MenuItem> myItems;
    };

    extern std::map<std::string, Menu> ourMenus;
};
#endif
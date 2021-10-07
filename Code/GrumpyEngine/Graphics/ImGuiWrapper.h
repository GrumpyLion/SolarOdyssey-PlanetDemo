#pragma once

#if !GE_FINAL

namespace ImGuiWrapper
{
    void Initialize();
    void Destroy();

    void PreRender();
    void Render();
};

#endif
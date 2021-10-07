#pragma once

#if !GE_FINAL

namespace Editor
{
    void Initialize();
    void Shutdown();

    void Render(float delta);
};

#endif
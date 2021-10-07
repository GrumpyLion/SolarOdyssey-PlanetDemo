#pragma once

struct RenderCommand;

namespace Renderer2D
{
    void Initialize();
    void Shutdown();

    void Submit(const RenderCommand& command);
    void Execute();
};
#pragma once

#include "imgui.h"

namespace CNode
{
    class PanViewController
    {
    public:
        bool Initialize();
        bool Draw(
            const ImVec2& viewportMin,
            const ImVec2& viewportMax
        );
        void Shutdown();

    private:
        unsigned int texture = 0;
        bool initialized = false;
    };
}
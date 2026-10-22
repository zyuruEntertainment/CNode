
#pragma once

#include "imgui.h"

#include <GLFW/glfw3.h>

namespace CNode
{
    class PanViewController
    {
    public:
        bool Initialize();

        bool Draw(
            GLFWwindow* window,
            const ImVec2& viewportMin,
            const ImVec2& viewportMax
        );

        void Shutdown();

    private:
        unsigned int texture = 0;
        bool initialized = false;
        bool cursorCaptured = false;
        GLFWwindow* capturedWindow = nullptr;
    };
}

#pragma once

#include "imgui.h"

#include <GLFW/glfw3.h>

namespace CNode
{
    class ZoomViewController
    {
    public:
        bool Initialize();

        bool Draw(
            GLFWwindow* window,
            const ImVec2& viewportMin,
            const ImVec2& viewportMax
        );

        float GetDragDeltaY() const;

        void Shutdown();

    private:
        unsigned int texture = 0;
        bool initialized = false;
        bool cursorCaptured = false;
        GLFWwindow* capturedWindow = nullptr;

        float dragDeltaY = 0.0f;
        int ignoreMouseDeltaFrames = 0;
    };
}
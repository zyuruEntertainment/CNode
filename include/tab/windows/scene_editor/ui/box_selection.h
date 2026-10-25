#pragma once

#include "imgui.h"

#include <GLFW/glfw3.h>

namespace CNode
{
    class BoxSelection
    {
    public:
        void Draw(
            GLFWwindow* window,
            const ImVec2& viewportMin,
            const ImVec2& viewportMax,
            bool viewportHovered,
            bool gizmoBusy,
            bool orbitGizmoActive,
            bool panControllerActive,
            bool zoomControllerActive,
            bool freeLookActive,
            bool panActive,
            bool orbitActive
        );

        bool IsSelecting() const;

    private:
        bool selecting = false;
        bool startedInViewport = false;

        ImVec2 startPos = ImVec2(0.0f, 0.0f);
        ImVec2 endPos = ImVec2(0.0f, 0.0f);
    };
}

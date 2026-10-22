#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include "imgui.h"

#include <array>

namespace CNode
{
    class OrbitGizmo
    {
    public:
        void Draw(
            SceneCamera& camera,
            const ImVec2& viewportMin,
            const ImVec2& viewportMax
        );

        bool IsBusy() const
        {
            return dragging || transitioning;
        }

    private:
        bool dragging = false;
        bool moved = false;
        bool ignoreFirstDragDelta = false;
        bool transitioning = false;
        bool cursorCaptured = false;

        ImVec2 lastMousePosition = ImVec2(0.0f, 0.0f);

        int selectedAxis = -1;
        float selectedSign = 1.0f;

        float sensitivity = 0.008f;

        float transitionElapsed = 0.0f;
        float transitionDuration = 0.28f;

        std::array<float, 3> transitionStartPosition{};
        std::array<float, 3> transitionTargetPosition{};

        std::array<float, 3> transitionStartUp{};
        std::array<float, 3> transitionTargetUp{};
    };
}
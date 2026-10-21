#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include "imgui.h"

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

    private:
        bool dragging = false;
        bool moved = false;
        bool ignoreFirstDragDelta = false;

        ImVec2 lastMousePosition = ImVec2(0.0f, 0.0f);

        int selectedAxis = -1;
        float selectedSign = 1.0f;

        float sensitivity = 0.008f;
    };
}

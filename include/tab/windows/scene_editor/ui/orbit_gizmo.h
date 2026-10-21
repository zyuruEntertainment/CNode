#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include "imgui.h"

namespace CNode
{
    void DrawOrbitGizmo(
        SceneCamera& camera,
        const ImVec2& viewportMin,
        const ImVec2& viewportMax
    );
}
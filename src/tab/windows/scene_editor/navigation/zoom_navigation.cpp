#include "tab/windows/scene_editor/navigation/zoom_navigation.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    void ZoomNavigation::Update(
        SceneCamera& camera,
        bool viewportHovered,
        bool controllerActive,
        float controllerDeltaY
    )
    {
        if (!controllerActive && !viewportHovered)
            return;

        if (!controllerActive &&
            (ImGui::IsMouseDown(ImGuiMouseButton_Left) ||
                ImGui::IsMouseDown(ImGuiMouseButton_Middle) ||
                ImGui::IsMouseDown(ImGuiMouseButton_Right)))
        {
            return;
        }

        const float offsetX =
            camera.position[0] - camera.target[0];

        const float offsetY =
            camera.position[1] - camera.target[1];

        const float offsetZ =
            camera.position[2] - camera.target[2];

        const float currentDistance = std::sqrt(
            offsetX * offsetX +
            offsetY * offsetY +
            offsetZ * offsetZ
        );

        if (currentDistance <= 0.0001f)
            return;

        float zoomFactor = 1.0f;

        if (controllerActive)
        {
            if (controllerDeltaY == 0.0f)
                return;

            zoomFactor = std::exp(
                controllerDeltaY * zoomSpeed
            );
        }
        else
        {
            const float wheelDelta =
                ImGui::GetIO().MouseWheel;

            if (wheelDelta == 0.0f)
                return;

            zoomFactor = std::exp(
                -wheelDelta * zoomSpeed
            );
        }

        const float newDistance = std::clamp(
            currentDistance * zoomFactor,
            minDistance,
            maxDistance
        );

        const float scale =
            newDistance / currentDistance;

        camera.position[0] =
            camera.target[0] + offsetX * scale;

        camera.position[1] =
            camera.target[1] + offsetY * scale;

        camera.position[2] =
            camera.target[2] + offsetZ * scale;
    }
}
#include "tab/windows/scene_editor/navigation/orbit_navigation.h"

#include "core/input/cursor_visibility.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    void OrbitNavigation::Update(
        SceneCamera& camera,
        GLFWwindow* window,
        bool viewportHovered
    )
    {
        constexpr int orbitButton = ImGuiMouseButton_Middle;
        constexpr float minRadius = 0.1f;
        constexpr float minPitch = -1.55334306f;
        constexpr float maxPitch = 1.55334306f;
        constexpr float epsilon = 0.00001f;
        constexpr int startupFramesToIgnore = 2;

        const ImGuiIO& io = ImGui::GetIO();

        if (ImGui::IsMouseClicked(orbitButton) &&
            viewportHovered &&
            !io.KeyShift &&
            !orbiting)
        {
            orbiting = true;
            ignoreMouseDeltaFrames = startupFramesToIgnore;

            const float offsetX =
                camera.position[0] - camera.target[0];

            const float offsetZ =
                camera.position[2] - camera.target[2];

            if (offsetX * offsetX + offsetZ * offsetZ > epsilon * epsilon)
            {
                lastYaw = std::atan2(offsetZ, offsetX);
                hasLastYaw = true;
            }

            CursorVisibility::SetCaptured(window, true);
            return;
        }

        if (!ImGui::IsMouseDown(orbitButton))
        {
            if (orbiting)
                CursorVisibility::SetCaptured(window, false);

            orbiting = false;
            ignoreMouseDeltaFrames = 0;
            return;
        }

        if (!orbiting)
            return;

        if (ignoreMouseDeltaFrames > 0)
        {
            --ignoreMouseDeltaFrames;
            return;
        }

        const float deltaX = io.MouseDelta.x;
        const float deltaY = io.MouseDelta.y;

        if (deltaX == 0.0f && deltaY == 0.0f)
            return;

        const float offsetX =
            camera.position[0] - camera.target[0];

        const float offsetY =
            camera.position[1] - camera.target[1];

        const float offsetZ =
            camera.position[2] - camera.target[2];

        const float radius = std::sqrt(
            offsetX * offsetX +
            offsetY * offsetY +
            offsetZ * offsetZ
        );

        if (radius < minRadius)
            return;

        const float horizontalRadius = std::sqrt(
            offsetX * offsetX +
            offsetZ * offsetZ
        );

        float yaw = lastYaw;

        if (horizontalRadius > epsilon)
        {
            yaw = std::atan2(offsetZ, offsetX);
            lastYaw = yaw;
            hasLastYaw = true;
        }
        else if (!hasLastYaw)
        {
            yaw = 0.0f;
            lastYaw = yaw;
            hasLastYaw = true;
        }

        float pitch = std::asin(
            std::clamp(offsetY / radius, -1.0f, 1.0f)
        );

        yaw += deltaX * sensitivity;

        pitch = std::clamp(
            pitch + deltaY * sensitivity,
            minPitch,
            maxPitch
        );

        lastYaw = yaw;

        const float cosPitch = std::cos(pitch);
        const float horizontal = radius * cosPitch;

        camera.position[0] =
            camera.target[0] + horizontal * std::cos(yaw);

        camera.position[1] =
            camera.target[1] + radius * std::sin(pitch);

        camera.position[2] =
            camera.target[2] + horizontal * std::sin(yaw);

        camera.up[0] = 0.0f;
        camera.up[1] = 1.0f;
        camera.up[2] = 0.0f;
    }
}
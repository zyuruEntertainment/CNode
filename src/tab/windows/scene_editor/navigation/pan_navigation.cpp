#include "tab/windows/scene_editor/navigation/pan_navigation.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    bool PanNavigation::Update(
        SceneCamera& camera,
        bool viewportHovered
    )
    {
        constexpr int panButton = ImGuiMouseButton_Middle;
        constexpr float minDistance = 0.0001f;

        const ImGuiIO& io = ImGui::GetIO();

        const bool middleDown =
            ImGui::IsMouseDown(panButton);

        const bool shiftDown = io.KeyShift;

        if (!panning &&
            ImGui::IsMouseClicked(panButton) &&
            shiftDown &&
            viewportHovered)
        {
            panning = true;

            return true;
        }

        if (!middleDown)
        {
            panning = false;
            return false;
        }

        if (!panning)
            return false;

        const float deltaX = io.MouseDelta.x;
        const float deltaY = io.MouseDelta.y;

        if (deltaX == 0.0f && deltaY == 0.0f)
            return true;

        float forwardX =
            camera.target[0] - camera.position[0];

        float forwardY =
            camera.target[1] - camera.position[1];

        float forwardZ =
            camera.target[2] - camera.position[2];

        const float distance = std::sqrt(
            forwardX * forwardX +
            forwardY * forwardY +
            forwardZ * forwardZ
        );

        if (distance < minDistance)
            return true;

        forwardX /= distance;
        forwardY /= distance;
        forwardZ /= distance;

        float rightX = -forwardZ;
        float rightY = 0.0f;
        float rightZ = forwardX;

        const float rightLength = std::sqrt(
            rightX * rightX +
            rightY * rightY +
            rightZ * rightZ
        );

        if (rightLength < minDistance)
            return true;

        rightX /= rightLength;
        rightY /= rightLength;
        rightZ /= rightLength;

        const float upX =
            rightY * forwardZ - rightZ * forwardY;

        const float upY =
            rightZ * forwardX - rightX * forwardZ;

        const float upZ =
            rightX * forwardY - rightY * forwardX;

        const float movementScale =
            distance * sensitivity;

        const float moveX =
            (-rightX * deltaX + upX * deltaY)
            * movementScale;

        const float moveY =
            (-rightY * deltaX + upY * deltaY)
            * movementScale;

        const float moveZ =
            (-rightZ * deltaX + upZ * deltaY)
            * movementScale;

        camera.position[0] += moveX;
        camera.position[1] += moveY;
        camera.position[2] += moveZ;

        camera.target[0] += moveX;
        camera.target[1] += moveY;
        camera.target[2] += moveZ;

        return true;
    }
}

#include "tab/windows/scene_editor/navigation/free_look_navigation.h"

#include "core/input/cursor_visibility.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    bool FreeLookNavigation::Update(
        SceneCamera& camera,
        GLFWwindow* window,
        bool viewportHovered
    )
    {
        constexpr float minFocusDistance = 0.1f;
        constexpr float minPitch = -1.55334306f;
        constexpr float maxPitch = 1.55334306f;
        constexpr int startupFramesToIgnore = 2;

        if (!window)
            return false;

        const bool rightDown =
            glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)
            == GLFW_PRESS;

        if (!active &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
            viewportHovered)
        {
            active = true;
            ignoreMouseDeltaFrames = startupFramesToIgnore;

            CursorVisibility::SetCaptured(window, true);
            return true;
        }

        if (!rightDown)
        {
            if (active)
                CursorVisibility::SetCaptured(window, false);

            active = false;
            ignoreMouseDeltaFrames = 0;
            return false;
        }

        if (!active)
            return false;

        const float deltaTime =
            std::max(ImGui::GetIO().DeltaTime, 0.0f);

        float forwardX =
            camera.target[0] - camera.position[0];
        float forwardY =
            camera.target[1] - camera.position[1];
        float forwardZ =
            camera.target[2] - camera.position[2];

        float focusDistance = std::sqrt(
            forwardX * forwardX +
            forwardY * forwardY +
            forwardZ * forwardZ
        );

        if (focusDistance < minFocusDistance)
            focusDistance = minFocusDistance;

        const float directionLength = std::sqrt(
            forwardX * forwardX +
            forwardY * forwardY +
            forwardZ * forwardZ
        );

        if (directionLength > 0.00001f)
        {
            forwardX /= directionLength;
            forwardY /= directionLength;
            forwardZ /= directionLength;
        }
        else
        {
            forwardX = -0.70710678f;
            forwardY = -0.40824829f;
            forwardZ = -0.57735027f;
        }

        float yaw = std::atan2(forwardZ, forwardX);
        float pitch = std::asin(
            std::clamp(forwardY, -1.0f, 1.0f)
        );

        if (ignoreMouseDeltaFrames > 0)
        {
            --ignoreMouseDeltaFrames;
        }
        else
        {
            const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;

            yaw += mouseDelta.x * lookSensitivity;
            pitch -= mouseDelta.y * lookSensitivity;
            pitch = std::clamp(pitch, minPitch, maxPitch);

            const float cosPitch = std::cos(pitch);

            forwardX = cosPitch * std::cos(yaw);
            forwardY = std::sin(pitch);
            forwardZ = cosPitch * std::sin(yaw);

            camera.target[0] =
                camera.position[0] + forwardX * focusDistance;
            camera.target[1] =
                camera.position[1] + forwardY * focusDistance;
            camera.target[2] =
                camera.position[2] + forwardZ * focusDistance;
        }

        const float rightX = -std::sin(yaw);
        const float rightZ = std::cos(yaw);

        float moveX = 0.0f;
        float moveY = 0.0f;
        float moveZ = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            moveX += std::cos(yaw);
            moveZ += std::sin(yaw);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            moveX -= std::cos(yaw);
            moveZ -= std::sin(yaw);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            moveX += rightX;
            moveZ += rightZ;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            moveX -= rightX;
            moveZ -= rightZ;
        }

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            moveY += 1.0f;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            moveY -= 1.0f;

        const float moveLength = std::sqrt(
            moveX * moveX +
            moveY * moveY +
            moveZ * moveZ
        );

        if (moveLength > 0.00001f)
        {
            moveX /= moveLength;
            moveY /= moveLength;
            moveZ /= moveLength;

            const bool shiftDown =
                glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

            const float speed =
                moveSpeed *
                (shiftDown ? fastMoveMultiplier : 1.0f) *
                deltaTime;

            moveX *= speed;
            moveY *= speed;
            moveZ *= speed;

            camera.position[0] += moveX;
            camera.position[1] += moveY;
            camera.position[2] += moveZ;

            camera.target[0] += moveX;
            camera.target[1] += moveY;
            camera.target[2] += moveZ;
        }

        return true;
    }
}

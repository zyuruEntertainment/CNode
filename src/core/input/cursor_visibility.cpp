#include "core/input/cursor_visibility.h"

#include <unordered_map>

namespace CNode
{
    namespace
    {
        struct MousePositionState
        {
            double x = 0.0;
            double y = 0.0;
            bool initialized = false;
        };

        std::unordered_map<GLFWwindow*, int> previousCursorModes;
        std::unordered_map<GLFWwindow*, int> requestedCursorModes;
        std::unordered_map<GLFWwindow*, bool> capturedWindows;
        std::unordered_map<GLFWwindow*, MousePositionState> mousePositions;
    }

    void CursorVisibility::SetVisible(
        GLFWwindow* window,
        bool visible
    )
    {
        if (!window)
            return;

        const int mode = visible
            ? GLFW_CURSOR_NORMAL
            : GLFW_CURSOR_HIDDEN;

        requestedCursorModes[window] = mode;

        ResetMousePosition(window);

        if (IsCaptured(window))
            return;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            mode
        );
    }

    void CursorVisibility::SetCaptured(
        GLFWwindow* window,
        bool captured
    )
    {
        if (!window)
            return;

        const bool currentlyCaptured = IsCaptured(window);

        if (captured)
        {
            if (currentlyCaptured)
                return;

            previousCursorModes[window] =
                glfwGetInputMode(window, GLFW_CURSOR);

            capturedWindows[window] = true;

            ResetMousePosition(window);

            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_DISABLED
            );

            return;
        }

        if (!currentlyCaptured)
            return;

        capturedWindows[window] = false;

        Restore(window);

        previousCursorModes.erase(window);

        ResetMousePosition(window);
    }

    bool CursorVisibility::IsCaptured(
        GLFWwindow* window
    )
    {
        if (!window)
            return false;

        const auto iterator = capturedWindows.find(window);

        return iterator != capturedWindows.end()
            && iterator->second;
    }

    bool CursorVisibility::ShouldBlockPointerInteraction(
        GLFWwindow* window
    )
    {
        if (!window)
            return false;

        if (IsCaptured(window))
            return true;

        const auto iterator = requestedCursorModes.find(window);

        return iterator != requestedCursorModes.end()
            && iterator->second == GLFW_CURSOR_HIDDEN;
    }

    void CursorVisibility::GetMouseDelta(
        GLFWwindow* window,
        float& deltaX,
        float& deltaY
    )
    {
        deltaX = 0.0f;
        deltaY = 0.0f;

        if (!window)
            return;

        double x = 0.0;
        double y = 0.0;

        glfwGetCursorPos(
            window,
            &x,
            &y
        );

        MousePositionState& state = mousePositions[window];

        if (state.initialized)
        {
            deltaX = static_cast<float>(x - state.x);
            deltaY = static_cast<float>(y - state.y);
        }

        state.x = x;
        state.y = y;
        state.initialized = true;
    }

    void CursorVisibility::Restore(
        GLFWwindow* window
    )
    {
        const auto requested = requestedCursorModes.find(window);

        if (requested != requestedCursorModes.end())
        {
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                requested->second
            );

            return;
        }

        const auto previous = previousCursorModes.find(window);

        if (previous != previousCursorModes.end())
        {
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                previous->second
            );
        }
        else
        {
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_NORMAL
            );
        }
    }

    void CursorVisibility::ResetMousePosition(
        GLFWwindow* window
    )
    {
        if (!window)
            return;

        mousePositions[window].initialized = false;
    }
}
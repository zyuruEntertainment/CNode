#include "core/input/cursor_visibility.h"

#include <unordered_map>

namespace CNode
{
    namespace
    {
        std::unordered_map<GLFWwindow*, int> previousCursorModes;
        std::unordered_map<GLFWwindow*, int> requestedCursorModes;
        std::unordered_map<GLFWwindow*, bool> capturedWindows;
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
}
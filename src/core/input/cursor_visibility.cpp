#include "core/input/cursor_visibility.h"

namespace CNode
{
    void CursorVisibility::SetVisible(
        GLFWwindow* window,
        bool visible
    )
    {
        if (!window)
            return;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            visible
            ? GLFW_CURSOR_NORMAL
            : GLFW_CURSOR_HIDDEN
        );
    }

    void CursorVisibility::SetCaptured(
        GLFWwindow* window,
        bool captured
    )
    {
        if (!window)
            return;

        glfwSetInputMode(
            window,
            GLFW_CURSOR,
            captured
            ? GLFW_CURSOR_DISABLED
            : GLFW_CURSOR_NORMAL
        );
    }
}
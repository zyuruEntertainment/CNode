#pragma once

#include <GLFW/glfw3.h>

namespace CNode
{
    class CursorVisibility
    {
    public:
        static void SetVisible(
            GLFWwindow* window,
            bool visible
        );

        static void SetCaptured(
            GLFWwindow* window,
            bool captured
        );

        static bool IsCaptured(
            GLFWwindow* window
        );

        static bool ShouldBlockPointerInteraction(
            GLFWwindow* window
        );

        static void GetMouseDelta(
            GLFWwindow* window,
            float& deltaX,
            float& deltaY
        );

    private:
        static void Restore(
            GLFWwindow* window
        );

        static void ResetMousePosition(
            GLFWwindow* window
        );
    };
}
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

    private:
        static void Restore(
            GLFWwindow* window
        );
    };
}
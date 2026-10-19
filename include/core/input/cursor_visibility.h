#pragma once

#include <GLFW/glfw3.h>

namespace CNode
{
    class CursorVisibility
    {
    public:
        // Tampilkan atau sembunyikan kursor.
        static void SetVisible(
            GLFWwindow* window,
            bool visible
        );

        // Tangkap kursor untuk navigasi seperti orbit.
        static void SetCaptured(
            GLFWwindow* window,
            bool captured
        );
    };
}
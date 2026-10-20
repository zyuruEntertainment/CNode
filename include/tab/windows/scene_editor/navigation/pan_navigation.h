#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include <GLFW/glfw3.h>

namespace CNode
{
    class PanNavigation
    {
    public:
        bool Update(
            SceneCamera& camera,
            GLFWwindow* window,
            bool viewportHovered
        );

    private:
        bool panning = false;
        int ignoreMouseDeltaFrames = 0;

        float sensitivity = 0.0015f;
    };
}

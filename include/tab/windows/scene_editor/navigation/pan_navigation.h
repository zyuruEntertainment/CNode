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
            bool viewportHovered,
            bool controllerActive = false
        );

    private:
        bool panning = false;
        bool controllerPanning = false;
        int ignoreMouseDeltaFrames = 0;

        float sensitivity = 0.0015f;
    };
}
#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include <GLFW/glfw3.h>

namespace CNode
{
    class OrbitNavigation
    {
    public:
        void Update(
            SceneCamera& camera,
            GLFWwindow* window,
            bool viewportHovered
        );

    private:
        bool orbiting = false;
        bool hasLastYaw = false;

        int ignoreMouseDeltaFrames = 0;

        float sensitivity = 0.005f;
        float lastYaw = 0.0f;
    };
}
#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include <GLFW/glfw3.h>

namespace CNode
{
    class FreeLookNavigation
    {
    public:
        bool Update(
            SceneCamera& camera,
            GLFWwindow* window,
            bool viewportHovered
        );

    private:
        bool active = false;
        int ignoreMouseDeltaFrames = 0;

        float lookSensitivity = 0.0025f;
        float moveSpeed = 5.0f;
        float fastMoveMultiplier = 4.0f;
    };
}

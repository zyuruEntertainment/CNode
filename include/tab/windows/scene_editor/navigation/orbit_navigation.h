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

        static void ApplyDragDelta(
            SceneCamera& camera,
            float deltaX,
            float deltaY,
            float sensitivity,
            float& lastYaw,
            bool& hasLastYaw
        );

    private:
        bool orbiting = false;
        bool hasLastYaw = false;

        int ignoreMouseDeltaFrames = 0;

        float sensitivity = 0.005f;
        float lastYaw = 0.0f;
    };
}
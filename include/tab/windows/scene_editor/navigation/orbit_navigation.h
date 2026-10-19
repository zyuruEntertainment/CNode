#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

namespace CNode
{
    class OrbitNavigation
    {
    public:
        void Update(
            SceneCamera& camera,
            bool viewportHovered
        );

    private:
        bool orbiting = false;

        float sensitivity = 0.005f;
    };
}
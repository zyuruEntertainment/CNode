#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

namespace CNode
{
    class PanNavigation
    {
    public:
        bool Update(
            SceneCamera& camera,
            bool viewportHovered
        );

    private:
        bool panning = false;

        float sensitivity = 0.0015f;
    };
}

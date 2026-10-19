#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

namespace CNode
{
    class ZoomNavigation
    {
    public:
        void Update(
            SceneCamera& camera,
            bool viewportHovered
        );

    private:
        float zoomSpeed = 0.15f;

        float minDistance = 0.25f;
        float maxDistance = 500.0f;
    };
}

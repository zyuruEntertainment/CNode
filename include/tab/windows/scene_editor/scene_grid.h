#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

#include <vector>

namespace CNode
{
    struct SceneLineVertex
    {
        float position[3];
        float color[3];
    };

    struct SceneGridVertices
    {
        std::vector<SceneLineVertex> minor;
        std::vector<SceneLineVertex> major;
    };

    SceneGridVertices BuildSceneGridVertices(
        const SceneCamera& camera,
        int viewportWidth,
        int viewportHeight
    );
}
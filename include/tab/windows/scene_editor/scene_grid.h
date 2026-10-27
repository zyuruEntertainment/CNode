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

    std::vector<SceneLineVertex> BuildSceneGridVertices(const SceneCamera& camera);
}
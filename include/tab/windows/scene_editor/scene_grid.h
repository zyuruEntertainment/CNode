#pragma once

#include <vector>

namespace CNode
{
    struct SceneLineVertex
    {
        float position[3];
        float color[3];
    };

    std::vector<SceneLineVertex> BuildSceneGridVertices();
}
#pragma once

namespace CNode
{
    struct SceneLineVertex
    {
        float position[3];
        float color[3];
    };

    struct GridSettings
    {
        bool showGrid = true;
        float opacity = 0.75f;
        float renderDistance = 400.0f;
    };
}

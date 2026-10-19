#include "tab/windows/scene_editor/scene_axis.h"

namespace CNode
{
    std::vector<SceneLineVertex> BuildSceneAxisVertices()
    {
        std::vector<SceneLineVertex> vertices;

        constexpr float axisLength = 10.0f;

        // x axis: merah
        vertices.push_back({
            { -axisLength, 0.0f, 0.0f },
            { 0.65f, 0.25f, 0.25f }
            });

        vertices.push_back({
            { axisLength, 0.0f, 0.0f },
            { 0.65f, 0.25f, 0.25f }
            });

        // z axis: biru
        vertices.push_back({
            { 0.0f, 0.0f, -axisLength },
            { 0.20f, 0.40f, 0.65f }
            });

        vertices.push_back({
            { 0.0f, 0.0f, axisLength },
            { 0.20f, 0.40f, 0.65f }
            });

        return vertices;
    }
}
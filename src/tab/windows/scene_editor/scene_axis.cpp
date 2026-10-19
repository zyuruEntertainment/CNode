
#include "tab/windows/scene_editor/scene_axis.h"

namespace CNode
{
    std::vector<SceneLineVertex> BuildSceneAxisVertices()
    {
        std::vector<SceneLineVertex> vertices;

        constexpr float axisLength = 10.0f;

        // X axis: merah
        vertices.push_back({
            { -axisLength, 0.0f, 0.0f },
            { 0.85f, 0.20f, 0.20f }
            });

        vertices.push_back({
            { axisLength, 0.0f, 0.0f },
            { 0.85f, 0.20f, 0.20f }
            });

        // Y axis: hijau (vertikal)
        vertices.push_back({
            { 0.0f, -axisLength, 0.0f },
            { 0.20f, 0.85f, 0.30f }
            });

        vertices.push_back({
            { 0.0f, axisLength, 0.0f },
            { 0.20f, 0.85f, 0.30f }
            });

        // Z axis: biru
        vertices.push_back({
            { 0.0f, 0.0f, -axisLength },
            { 0.20f, 0.40f, 0.90f }
            });

        vertices.push_back({
            { 0.0f, 0.0f, axisLength },
            { 0.20f, 0.40f, 0.90f }
            });

        return vertices;
    }
}

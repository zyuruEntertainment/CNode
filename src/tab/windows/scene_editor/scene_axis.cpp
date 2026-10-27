#include "tab/windows/scene_editor/scene_axis.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    std::vector<SceneLineVertex> BuildSceneAxisVertices(const SceneCamera& camera)
    {
        std::vector<SceneLineVertex> vertices;

        const float dx =
            camera.position[0] - camera.target[0];
        const float dy =
            camera.position[1] - camera.target[1];
        const float dz =
            camera.position[2] - camera.target[2];

        const float distance = std::sqrt(
            dx * dx + dy * dy + dz * dz
        );

        const float axisLength =
            std::max(1000.0f, distance * 4.0f);

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
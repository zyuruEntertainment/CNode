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

        // X axis: red
        vertices.push_back({
            { -axisLength, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f }
            });

        vertices.push_back({
            { axisLength, 0.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f }
            });

        // Y axis: green (vertical)
        vertices.push_back({
            { 0.0f, -axisLength, 0.0f },
            { 0.0f, 1.0f, 0.0f }
            });

        vertices.push_back({
            { 0.0f, axisLength, 0.0f },
            { 0.0f, 1.0f, 0.0f }
            });

        // Z axis: blue
        vertices.push_back({
            { 0.0f, 0.0f, -axisLength },
            { 0.0f, 0.0f, 1.0f }
            });

        vertices.push_back({
            { 0.0f, 0.0f, axisLength },
            { 0.0f, 0.0f, 1.0f }
            });

        return vertices;
    }
}
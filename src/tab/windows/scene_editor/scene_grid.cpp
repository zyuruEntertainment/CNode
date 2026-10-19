#include "tab/windows/scene_editor/scene_grid.h"

namespace CNode
{
    namespace
    {
        void AddLine(
            std::vector<SceneLineVertex>& vertices,
            float x1, float y1, float z1,
            float x2, float y2, float z2)
        {
            constexpr float color = 0.25f;

            vertices.push_back({
                { x1, y1, z1 },
                { color, color, color }
                });

            vertices.push_back({
                { x2, y2, z2 },
                { color, color, color }
                });
        }
    }

    std::vector<SceneLineVertex> BuildSceneGridVertices()
    {
        std::vector<SceneLineVertex> vertices;

        constexpr int gridSize = 10;

        for (int i = -gridSize; i <= gridSize; ++i)
        {
            if (i == 0)
                continue;

            const float p = static_cast<float>(i);

            AddLine(
                vertices,
                p, 0.0f, -static_cast<float>(gridSize),
                p, 0.0f, static_cast<float>(gridSize)
            );

            AddLine(
                vertices,
                -static_cast<float>(gridSize), 0.0f, p,
                static_cast<float>(gridSize), 0.0f, p
            );
        }

        return vertices;
    }
}
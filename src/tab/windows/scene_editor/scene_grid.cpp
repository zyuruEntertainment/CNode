#include "tab/windows/scene_editor/scene_grid.h"
#include "tab/windows/project_setting/project_setting.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    namespace
    {
        float FadeForPoint(
            float x,
            float z,
            float centerX,
            float centerZ,
            float radius)
        {
            const float dx = x - centerX;
            const float dz = z - centerZ;
            const float distance = std::sqrt(dx * dx + dz * dz);
            float fade = 1.0f - (distance / radius);
            fade = std::clamp(fade, 0.0f, 1.0f);
            return fade * fade;
        }
    }

    std::vector<SceneLineVertex> BuildSceneGridVertices(const SceneCamera& camera)
    {
        std::vector<SceneLineVertex> vertices;

        const float cameraX = camera.position[0];
        const float cameraZ = camera.position[2];

        const float targetX = camera.target[0];
        const float targetY = camera.target[1];
        const float targetZ = camera.target[2];

        const float dx = cameraX - targetX;
        const float dy = camera.position[1] - targetY;
        const float dz = cameraZ - targetZ;

        const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

        float spacing = 1.0f;

        if (distance > 200.0f)
            spacing = 10.0f;
        else if (distance > 50.0f)
            spacing = 5.0f;
        else if (distance > 20.0f)
            spacing = 2.0f;

        const float radius = std::max(30.0f, GetProjectSettings().gridRenderDistance);

        const float originX =
            std::floor(cameraX / spacing) * spacing;

        const float originZ =
            std::floor(cameraZ / spacing) * spacing;

        const int stepCount =
            static_cast<int>(radius / spacing);

        constexpr float baseColor = 0.28f;

        for (int i = -stepCount; i <= stepCount; ++i)
        {
            const float x = originX + static_cast<float>(i) * spacing;

            const float deltaX = x - originX;

            if (std::fabs(deltaX) > radius)
                continue;

            if (std::fabs(x) < spacing * 0.5f)
                continue;

            const float halfExtent = std::sqrt(
                std::max(0.0f, radius * radius - deltaX * deltaX)
            );

            const float zStart = originZ - halfExtent;
            const float zEnd = originZ + halfExtent;

            const float fadeStart = FadeForPoint(
                x, zStart, originX, originZ, radius
            );

            const float fadeEnd = FadeForPoint(
                x, zEnd, originX, originZ, radius
            );

            const float colorStart = baseColor * fadeStart;
            const float colorEnd = baseColor * fadeEnd;

            vertices.push_back({
                { x, 0.0f, zStart },
                { colorStart, colorStart, colorStart }
                });

            vertices.push_back({
                { x, 0.0f, zEnd },
                { colorEnd, colorEnd, colorEnd }
                });
        }

        for (int i = -stepCount; i <= stepCount; ++i)
        {
            const float z = originZ + static_cast<float>(i) * spacing;

            const float deltaZ = z - originZ;

            if (std::fabs(deltaZ) > radius)
                continue;

            if (std::fabs(z) < spacing * 0.5f)
                continue;

            const float halfExtent = std::sqrt(
                std::max(0.0f, radius * radius - deltaZ * deltaZ)
            );

            const float xStart = originX - halfExtent;
            const float xEnd = originX + halfExtent;

            const float fadeStart = FadeForPoint(
                xStart, z, originX, originZ, radius
            );

            const float fadeEnd = FadeForPoint(
                xEnd, z, originX, originZ, radius
            );

            const float colorStart = baseColor * fadeStart;
            const float colorEnd = baseColor * fadeEnd;

            vertices.push_back({
                { xStart, 0.0f, z },
                { colorStart, colorStart, colorStart }
                });

            vertices.push_back({
                { xEnd, 0.0f, z },
                { colorEnd, colorEnd, colorEnd }
                });
        }

        return vertices;
    }
}
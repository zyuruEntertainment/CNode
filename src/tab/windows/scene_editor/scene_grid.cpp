#include "tab/windows/scene_editor/scene_grid.h"
#include "tab/windows/project_setting/project_setting.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    namespace
    {
        float SmoothFade(float distance, float radius)
        {
            if (radius <= 0.0f)
                return 0.0f;

            const float t = std::clamp(distance / radius, 0.0f, 1.0f);
            const float fade = 1.0f - t;
            return fade * fade * (3.0f - 2.0f * fade);
        }

        void BlendColor(
            float fade,
            float opacity,
            const float* background,
            float gridColor,
            float* color)
        {
            for (int i = 0; i < 3; ++i)
            {
                color[i] =
                    background[i] +
                    (gridColor - background[i]) *
                    opacity * fade;
            }
        }

        void AddGridLine(
            std::vector<SceneLineVertex>& vertices,
            float x1, float y1, float z1,
            float x2, float y2, float z2,
            const float* color)
        {
            vertices.push_back({
                { x1, y1, z1 },
                { color[0], color[1], color[2] }
                });

            vertices.push_back({
                { x2, y2, z2 },
                { color[0], color[1], color[2] }
                });
        }
    }

    SceneGridVertices BuildSceneGridVertices(
        const SceneCamera& camera,
        int viewportWidth,
        int viewportHeight)
    {
        SceneGridVertices result;

        const float cameraX = camera.position[0];
        const float cameraZ = camera.position[2];

        const float targetX = camera.target[0];
        const float targetY = camera.target[1];
        const float targetZ = camera.target[2];

        const float dx = cameraX - targetX;
        const float dy = camera.position[1] - targetY;
        const float dz = cameraZ - targetZ;

        const float distanceToTarget = std::sqrt(
            dx * dx + dy * dy + dz * dz
        );

        viewportWidth = std::max(viewportWidth, 1);
        viewportHeight = std::max(viewportHeight, 1);

        constexpr float Pi = 3.14159265358979323846f;
        const float fovRadians =
            camera.fieldOfView * Pi / 180.0f;

        const float pixelsPerUnit =
            (static_cast<float>(viewportHeight) * 0.5f) /
            (std::tan(fovRadians * 0.5f) *
             std::max(distanceToTarget, 1.0f));

        const float desiredPixels = 12.0f;
        float rawSpacing = desiredPixels /
            std::max(pixelsPerUnit, 0.001f);

        float spacing = 1.0f;

        while (spacing * 2.0f <= rawSpacing &&
               spacing * 2.0f <= 100.0f)
        {
            spacing *= 2.0f;
        }

        spacing = std::clamp(spacing, 1.0f, 100.0f);

        const ProjectSettings& projectSettings =
            GetProjectSettings();

        const float opacity = projectSettings.gridOpacity;
        const float radius = std::max(
            30.0f,
            projectSettings.gridRenderDistance
        );

        const float originX =
            std::floor(cameraX / spacing) * spacing;

        const float originZ =
            std::floor(cameraZ / spacing) * spacing;

        const int stepCount =
            static_cast<int>(radius / spacing);

        constexpr float minorGridColor = 0.22f;
        constexpr float majorGridColor = 0.42f;

        for (int i = -stepCount; i <= stepCount; ++i)
        {
            const float x = originX +
                static_cast<float>(i) * spacing;

            const float deltaX = x - originX;

            if (std::fabs(deltaX) > radius)
                continue;

            if (std::fabs(x) < spacing * 0.5f)
                continue;

            const float halfExtent = std::sqrt(
                std::max(
                    0.0f,
                    radius * radius - deltaX * deltaX
                )
            );

            const float zStart = originZ - halfExtent;
            const float zEnd = originZ + halfExtent;

            const float centerDistance = std::fabs(deltaX);
            const float fade = SmoothFade(
                centerDistance, radius
            );

            const bool major = (i % 10 == 0);

            float color[3] = {};
            BlendColor(
                fade,
                opacity,
                projectSettings.backgroundColor,
                major ? majorGridColor : minorGridColor,
                color
            );

            std::vector<SceneLineVertex>& target =
                major ? result.major : result.minor;

            AddGridLine(
                target,
                x, 0.0f, zStart,
                x, 0.0f, zEnd,
                color
            );
        }

        for (int i = -stepCount; i <= stepCount; ++i)
        {
            const float z = originZ +
                static_cast<float>(i) * spacing;

            const float deltaZ = z - originZ;

            if (std::fabs(deltaZ) > radius)
                continue;

            if (std::fabs(z) < spacing * 0.5f)
                continue;

            const float halfExtent = std::sqrt(
                std::max(
                    0.0f,
                    radius * radius - deltaZ * deltaZ
                )
            );

            const float xStart = originX - halfExtent;
            const float xEnd = originX + halfExtent;

            const float centerDistance = std::fabs(deltaZ);
            const float fade = SmoothFade(
                centerDistance, radius
            );

            const bool major = (i % 10 == 0);

            float color[3] = {};
            BlendColor(
                fade,
                opacity,
                projectSettings.backgroundColor,
                major ? majorGridColor : minorGridColor,
                color
            );

            std::vector<SceneLineVertex>& target =
                major ? result.major : result.minor;

            AddGridLine(
                target,
                xStart, 0.0f, z,
                xEnd, 0.0f, z,
                color
            );
        }

        return result;
    }
}
#include "tab/windows/scene_editor/scene_grid.h"
#include "tab/windows/project_setting/project_setting_data.h"

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
                    opacity * std::clamp(fade, 0.0f, 1.0f);
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

        const float desiredPixels = 24.0f;
        float rawSpacing = desiredPixels /
            std::max(pixelsPerUnit, 0.001f);

        float spacing = 1.0f;

        while (spacing < rawSpacing &&
               spacing * 2.0f <= 100.0f)
        {
            spacing *= 2.0f;
        }

        spacing = std::clamp(spacing, 1.0f, 100.0f);

        float viewDirX = targetX - cameraX;
        float viewDirZ = targetZ - cameraZ;
        const float viewDirLength = std::sqrt(
            viewDirX * viewDirX + viewDirZ * viewDirZ
        );

        if (viewDirLength > 0.0001f)
        {
            viewDirX /= viewDirLength;
            viewDirZ /= viewDirLength;
        }
        else
        {
            viewDirX = 0.0f;
            viewDirZ = 1.0f;
        }

        // Reduce the family of lines that becomes visually stacked
        // when the camera looks close to that axis.
        const float xFamilyScale = std::abs(viewDirZ);
        const float zFamilyScale = std::abs(viewDirX);

        const ProjectSettings& projectSettings =
            GetProjectSettings();

        const float opacity = projectSettings.gridOpacity;
        const float radius = std::max(
            30.0f,
            projectSettings.gridRenderDistance
        );

        const int minI = static_cast<int>(
            std::floor((cameraX - radius) / spacing)
        );

        const int maxI = static_cast<int>(
            std::ceil((cameraX + radius) / spacing)
        );

        constexpr float minorGridColor = 0.22f;
        constexpr float majorGridColor = 0.42f;

        for (int i = minI; i <= maxI; ++i)
        {
            const float x = static_cast<float>(i) * spacing;

            const float deltaX = x - cameraX;

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

            const float zStart = cameraZ - halfExtent;
            const float zEnd = cameraZ + halfExtent;
            const float closestZ = std::clamp(
                cameraZ, zStart, zEnd
            );

            const bool major = (i % 10 == 0);
            const float gridColor =
                major ? majorGridColor : minorGridColor;

            std::vector<SceneLineVertex>& target =
                major ? result.major : result.minor;

            const float fadeAtClosest =
                SmoothFade(std::fabs(deltaX), radius) *
                xFamilyScale;

            float colorAtClosest[3] = {};
            BlendColor(
                fadeAtClosest,
                opacity,
                projectSettings.backgroundColor,
                gridColor,
                colorAtClosest
            );

            if (closestZ > zStart + 0.0001f)
            {
                const float distanceAtStart = std::sqrt(
                    deltaX * deltaX +
                    (zStart - cameraZ) * (zStart - cameraZ)
                );

                const float fadeAtStart =
                    SmoothFade(distanceAtStart, radius) *
                    xFamilyScale;

                float colorAtStart[3] = {};
                BlendColor(
                    fadeAtStart,
                    opacity,
                    projectSettings.backgroundColor,
                    gridColor,
                    colorAtStart
                );

                AddGridLine(
                    target,
                    x, 0.0f, zStart,
                    x, 0.0f, closestZ,
                    colorAtStart
                );

                target.back().color[0] = colorAtClosest[0];
                target.back().color[1] = colorAtClosest[1];
                target.back().color[2] = colorAtClosest[2];
            }

            if (zEnd > closestZ + 0.0001f)
            {
                const float distanceAtEnd = std::sqrt(
                    deltaX * deltaX +
                    (zEnd - cameraZ) * (zEnd - cameraZ)
                );

                const float fadeAtEnd =
                    SmoothFade(distanceAtEnd, radius) *
                    xFamilyScale;

                float colorAtEnd[3] = {};
                BlendColor(
                    fadeAtEnd,
                    opacity,
                    projectSettings.backgroundColor,
                    gridColor,
                    colorAtEnd
                );

                AddGridLine(
                    target,
                    x, 0.0f, closestZ,
                    x, 0.0f, zEnd,
                    colorAtClosest
                );

                target.back().color[0] = colorAtEnd[0];
                target.back().color[1] = colorAtEnd[1];
                target.back().color[2] = colorAtEnd[2];
            }
        }

        const int minJ = static_cast<int>(
            std::floor((cameraZ - radius) / spacing)
        );

        const int maxJ = static_cast<int>(
            std::ceil((cameraZ + radius) / spacing)
        );

        for (int j = minJ; j <= maxJ; ++j)
        {
            const float z = static_cast<float>(j) * spacing;

            const float deltaZ = z - cameraZ;

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

            const float xStart = cameraX - halfExtent;
            const float xEnd = cameraX + halfExtent;
            const float closestX = std::clamp(
                cameraX, xStart, xEnd
            );

            const bool major = (j % 10 == 0);
            const float gridColor =
                major ? majorGridColor : minorGridColor;

            std::vector<SceneLineVertex>& target =
                major ? result.major : result.minor;

            const float fadeAtClosest =
                SmoothFade(std::fabs(deltaZ), radius) *
                zFamilyScale;

            float colorAtClosest[3] = {};
            BlendColor(
                fadeAtClosest,
                opacity,
                projectSettings.backgroundColor,
                gridColor,
                colorAtClosest
            );

            if (closestX > xStart + 0.0001f)
            {
                const float distanceAtStart = std::sqrt(
                    (xStart - cameraX) * (xStart - cameraX) +
                    deltaZ * deltaZ
                );

                const float fadeAtStart =
                    SmoothFade(distanceAtStart, radius) *
                    zFamilyScale;

                float colorAtStart[3] = {};
                BlendColor(
                    fadeAtStart,
                    opacity,
                    projectSettings.backgroundColor,
                    gridColor,
                    colorAtStart
                );

                AddGridLine(
                    target,
                    xStart, 0.0f, z,
                    closestX, 0.0f, z,
                    colorAtStart
                );

                target.back().color[0] = colorAtClosest[0];
                target.back().color[1] = colorAtClosest[1];
                target.back().color[2] = colorAtClosest[2];
            }

            if (xEnd > closestX + 0.0001f)
            {
                const float distanceAtEnd = std::sqrt(
                    (xEnd - cameraX) * (xEnd - cameraX) +
                    deltaZ * deltaZ
                );

                const float fadeAtEnd =
                    SmoothFade(distanceAtEnd, radius) *
                    zFamilyScale;

                float colorAtEnd[3] = {};
                BlendColor(
                    fadeAtEnd,
                    opacity,
                    projectSettings.backgroundColor,
                    gridColor,
                    colorAtEnd
                );

                AddGridLine(
                    target,
                    closestX, 0.0f, z,
                    xEnd, 0.0f, z,
                    colorAtClosest
                );

                target.back().color[0] = colorAtEnd[0];
                target.back().color[1] = colorAtEnd[1];
                target.back().color[2] = colorAtEnd[2];
            }
        }

        return result;
    }
}
#include "tab/windows/scene_editor/ui/orbit_gizmo.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    namespace
    {
        constexpr float Pi = 3.14159265358979323846f;

        struct Vec3
        {
            float x, y, z;
        };

        Vec3 Subtract(const Vec3& a, const Vec3& b)
        {
            return {
                a.x - b.x,
                a.y - b.y,
                a.z - b.z
            };
        }

        Vec3 Cross(const Vec3& a, const Vec3& b)
        {
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
        }

        float Dot(const Vec3& a, const Vec3& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        Vec3 Normalize(const Vec3& v)
        {
            const float length = std::sqrt(Dot(v, v));

            if (length < 0.00001f)
                return { 0.0f, 0.0f, 0.0f };

            return {
                v.x / length,
                v.y / length,
                v.z / length
            };
        }

        ImVec2 ProjectAxis(
            const Vec3& axis,
            const Vec3& right,
            const Vec3& up,
            const ImVec2& center,
            float radius
        )
        {
            return ImVec2(
                center.x + Dot(axis, right) * radius,
                center.y - Dot(axis, up) * radius
            );
        }

        float DistanceSquared(
            const ImVec2& a,
            const ImVec2& b
        )
        {
            const float x = a.x - b.x;
            const float y = a.y - b.y;

            return x * x + y * y;
        }

        void SnapToAxis(
            SceneCamera& camera,
            const Vec3& direction
        )
        {
            const Vec3 target = {
                camera.target[0],
                camera.target[1],
                camera.target[2]
            };

            const Vec3 position = {
                camera.position[0],
                camera.position[1],
                camera.position[2]
            };

            const Vec3 offset = Subtract(position, target);

            const float distance = std::max(
                std::sqrt(Dot(offset, offset)),
                1.0f
            );

            camera.position[0] =
                target.x + direction.x * distance;

            camera.position[1] =
                target.y + direction.y * distance;

            camera.position[2] =
                target.z + direction.z * distance;

            // gunakan z sebagai arah atas untuk tampilan top/bottom.
            if (std::abs(direction.y) > 0.99f)
            {
                camera.up[0] = 0.0f;
                camera.up[1] = 0.0f;
                camera.up[2] = 1.0f;
            }
            else
            {
                camera.up[0] = 0.0f;
                camera.up[1] = 1.0f;
                camera.up[2] = 0.0f;
            }
        }
    }

    void DrawOrbitGizmo(
        SceneCamera& camera,
        const ImVec2& viewportMin,
        const ImVec2& viewportMax
    )
    {
        constexpr float gizmoRadius = 34.0f;
        constexpr float hitRadius = 12.0f;

        const ImVec2 center(
            viewportMax.x - 52.0f,
            viewportMin.y + 52.0f
        );

        if (center.x - 48.0f < viewportMin.x ||
            center.y + 48.0f > viewportMax.y)
        {
            return;
        }

        const Vec3 position = {
            camera.position[0],
            camera.position[1],
            camera.position[2]
        };

        const Vec3 target = {
            camera.target[0],
            camera.target[1],
            camera.target[2]
        };

        const Vec3 forward = Normalize(
            Subtract(target, position)
        );

        const Vec3 worldUp = { 0.0f, 1.0f, 0.0f };

        Vec3 right = Normalize(Cross(forward, worldUp));

        if (Dot(right, right) < 0.0001f)
            right = { 1.0f, 0.0f, 0.0f };

        const Vec3 up = Normalize(Cross(right, forward));

        const Vec3 axes[] = {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
        };

        const ImU32 colors[] = {
            IM_COL32(235, 75, 75, 255),
            IM_COL32(95, 205, 115, 255),
            IM_COL32(80, 145, 255, 255)
        };

        const char* labels[] = { "X", "Y", "Z" };

        ImDrawList* drawList =
            ImGui::GetWindowDrawList();

        drawList->AddCircleFilled(
            center,
            40.0f,
            IM_COL32(25, 27, 31, 220),
            32
        );

        drawList->AddCircle(
            center,
            40.0f,
            IM_COL32(90, 95, 105, 220),
            32,
            1.0f
        );

        ImGuiIO& io = ImGui::GetIO();

        const ImVec2 mouse = io.MousePos;
        const bool clicked = ImGui::IsMouseClicked(
            ImGuiMouseButton_Left
        );

        float closestDistance = hitRadius * hitRadius;
        int selectedAxis = -1;
        float selectedSign = 1.0f;

        for (int i = 0; i < 3; ++i)
        {
            for (int sign : { -1, 1 })
            {
                const Vec3 direction = {
                    axes[i].x * sign,
                    axes[i].y * sign,
                    axes[i].z * sign
                };

                const ImVec2 endpoint = ProjectAxis(
                    direction,
                    right,
                    up,
                    center,
                    gizmoRadius
                );

                const float depth = Dot(direction, forward);

                const ImU32 color = depth > 0.0f
                    ? colors[i]
                    : IM_COL32(100, 105, 115, 180);

                drawList->AddLine(
                    center,
                    endpoint,
                    color,
                    depth > 0.0f ? 3.0f : 2.0f
                );

                drawList->AddCircleFilled(
                    endpoint,
                    depth > 0.0f ? 7.0f : 5.0f,
                    color,
                    20
                );

                const ImVec2 labelPosition(
                    endpoint.x - 4.0f,
                    endpoint.y - 7.0f
                );

                drawList->AddText(
                    labelPosition,
                    IM_COL32(255, 255, 255, 255),
                    sign > 0 ? labels[i] : labels[i]
                );

                const float distance = DistanceSquared(
                    mouse,
                    endpoint
                );

                if (distance < closestDistance)
                {
                    closestDistance = distance;
                    selectedAxis = i;
                    selectedSign = static_cast<float>(sign);
                }
            }
        }

        if (clicked && selectedAxis >= 0)
        {
            const Vec3 direction = {
                axes[selectedAxis].x * selectedSign,
                axes[selectedAxis].y * selectedSign,
                axes[selectedAxis].z * selectedSign
            };

            SnapToAxis(camera, direction);
        }
    }
}
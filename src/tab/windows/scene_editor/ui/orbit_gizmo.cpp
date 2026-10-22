#include "tab/windows/scene_editor/ui/orbit_gizmo.h"

#include "core/input/cursor_visibility.h"

#include "imgui.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <cmath>

namespace CNode
{
    namespace
    {
        constexpr float Pi = 3.14159265358979323846f;

        constexpr float GizmoRadius = 40.0f;
        constexpr float InteractionRadius = 52.0f;
        constexpr float HitRadius = 13.0f;

        constexpr float AxisDotRadius = 10.0f;
        constexpr float BackAxisDotRadius = 5.5f;

        constexpr float GizmoBackgroundRadius = 48.0f;
        constexpr float GizmoBorderThickness = 1.2f;

        constexpr ImU32 BackgroundColor =
            IM_COL32(25, 27, 32, 235);

        constexpr ImU32 BorderColor =
            IM_COL32(82, 88, 99, 230);

        constexpr ImU32 TextColor =
            IM_COL32(245, 247, 250, 255);

        struct Vec3
        {
            float x;
            float y;
            float z;
        };

        constexpr std::array<Vec3, 3> Axes = { {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
        } };

        constexpr std::array<ImU32, 3> AxisColors = { {
            IM_COL32(238, 83, 83, 255),
            IM_COL32(91, 211, 126, 255),
            IM_COL32(83, 151, 255, 255)
        } };

        constexpr std::array<const char*, 3> AxisLabels = { {
            "X", "Y", "Z"
        } };

        float Dot(const Vec3& a, const Vec3& b)
        {
            return
                a.x * b.x +
                a.y * b.y +
                a.z * b.z;
        }

        Vec3 Cross(const Vec3& a, const Vec3& b)
        {
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
        }

        Vec3 Normalize(const Vec3& value)
        {
            const float length = std::sqrt(Dot(value, value));

            if (length < 0.00001f)
            {
                return { 0.0f, 0.0f, 0.0f };
            }

            return {
                value.x / length,
                value.y / length,
                value.z / length
            };
        }

        Vec3 Lerp(
            const Vec3& from,
            const Vec3& to,
            float t
        )
        {
            return {
                from.x + (to.x - from.x) * t,
                from.y + (to.y - from.y) * t,
                from.z + (to.z - from.z) * t
            };
        }

        float EaseInOut(float t)
        {
            t = std::clamp(t, 0.0f, 1.0f);
            return t * t * (3.0f - 2.0f * t);
        }

        Vec3 ReadVector(const float value[3])
        {
            return { value[0], value[1], value[2] };
        }

        void WriteVector(float destination[3], const Vec3& value)
        {
            destination[0] = value.x;
            destination[1] = value.y;
            destination[2] = value.z;
        }

        Vec3 ReadArray(const std::array<float, 3>& value)
        {
            return { value[0], value[1], value[2] };
        }

        void WriteArray(
            std::array<float, 3>& destination,
            const Vec3& value
        )
        {
            destination[0] = value.x;
            destination[1] = value.y;
            destination[2] = value.z;
        }

        ImVec2 ProjectAxis(
            const Vec3& axis,
            const Vec3& right,
            const Vec3& up,
            const ImVec2& center,
            float radius
        )
        {
            return {
                center.x + Dot(axis, right) * radius,
                center.y - Dot(axis, up) * radius
            };
        }

        float DistanceSquared(
            const ImVec2& a,
            const ImVec2& b
        )
        {
            const float dx = a.x - b.x;
            const float dy = a.y - b.y;

            return dx * dx + dy * dy;
        }

        bool IsInside(
            const ImVec2& point,
            const ImVec2& center,
            float radius
        )
        {
            return DistanceSquared(point, center) <= radius * radius;
        }

        ImU32 DimColor(ImU32 color)
        {
            const int r = (color >> IM_COL32_R_SHIFT) & 0xFF;
            const int g = (color >> IM_COL32_G_SHIFT) & 0xFF;
            const int b = (color >> IM_COL32_B_SHIFT) & 0xFF;

            return IM_COL32(
                r * 65 / 100,
                g * 65 / 100,
                b * 65 / 100,
                175
            );
        }

        void GetCameraBasis(
            const SceneCamera& camera,
            Vec3& forward,
            Vec3& right,
            Vec3& up
        )
        {
            const Vec3 target = {
                camera.target[0],
                camera.target[1],
                camera.target[2]
            };

            const Vec3 offset = {
                camera.position[0] - target.x,
                camera.position[1] - target.y,
                camera.position[2] - target.z
            };

            forward = Normalize({
                -offset.x,
                -offset.y,
                -offset.z
                });

            const Vec3 worldUp = { 0.0f, 1.0f, 0.0f };

            right = Normalize(Cross(forward, worldUp));

            if (Dot(right, right) < 0.0001f)
            {
                right = { 1.0f, 0.0f, 0.0f };
            }

            up = Normalize(Cross(right, forward));
        }
    }

    void OrbitGizmo::Draw(
        SceneCamera& camera,
        const ImVec2& viewportMin,
        const ImVec2& viewportMax
    )
    {
        GLFWwindow* window = glfwGetCurrentContext();

        const ImVec2 center(
            viewportMax.x - 52.0f,
            viewportMin.y + 52.0f
        );

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        if (center.x - InteractionRadius < viewportMin.x ||
            center.y + InteractionRadius > viewportMax.y)
        {
            if (cursorCaptured)
            {
                CursorVisibility::SetCaptured(window, false);
                cursorCaptured = false;
            }

            dragging = false;
            moved = false;
            ignoreFirstDragDelta = false;
            selectedAxis = -1;

            return;
        }

        const Vec3 target = {
            camera.target[0],
            camera.target[1],
            camera.target[2]
        };

        if (transitioning)
        {
            transitionElapsed += ImGui::GetIO().DeltaTime;

            const float progress = std::clamp(
                transitionElapsed / transitionDuration,
                0.0f,
                1.0f
            );

            const float eased = EaseInOut(progress);

            const Vec3 startPosition =
                ReadArray(transitionStartPosition);

            const Vec3 targetPosition =
                ReadArray(transitionTargetPosition);

            const Vec3 startUp =
                ReadArray(transitionStartUp);

            const Vec3 targetUp =
                ReadArray(transitionTargetUp);

            const Vec3 position = Lerp(
                startPosition,
                targetPosition,
                eased
            );

            Vec3 up = Normalize(Lerp(
                startUp,
                targetUp,
                eased
            ));

            const Vec3 viewDirection = Normalize({
                target.x - position.x,
                target.y - position.y,
                target.z - position.z
                });

            if (Dot(up, up) < 0.0001f ||
                std::abs(Dot(up, viewDirection)) > 0.995f)
            {
                up = targetUp;
            }

            WriteVector(camera.position, position);
            WriteVector(camera.up, up);

            if (progress >= 1.0f)
            {
                WriteVector(camera.position, targetPosition);
                WriteVector(camera.up, targetUp);

                transitioning = false;
                transitionElapsed = 0.0f;
            }
        }

        Vec3 forward;
        Vec3 right;
        Vec3 up;

        GetCameraBasis(camera, forward, right, up);

        const ImVec2 mouse = ImGui::GetIO().MousePos;

        const bool hovered = IsInside(
            mouse,
            center,
            InteractionRadius
        );

        const bool canInteract = !transitioning;

        if (canInteract &&
            !dragging &&
            hovered &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            dragging = true;
            moved = false;
            ignoreFirstDragDelta = true;

            lastMousePosition = mouse;
            selectedAxis = -1;

            float closestDistance = HitRadius * HitRadius;

            for (int i = 0; i < 3; ++i)
            {
                for (int sign : { -1, 1 })
                {
                    const Vec3 direction = {
                        Axes[i].x * sign,
                        Axes[i].y * sign,
                        Axes[i].z * sign
                    };

                    const ImVec2 endpoint = ProjectAxis(
                        direction,
                        right,
                        up,
                        center,
                        GizmoRadius
                    );

                    const float distanceSquared =
                        DistanceSquared(mouse, endpoint);

                    if (distanceSquared < closestDistance)
                    {
                        closestDistance = distanceSquared;
                        selectedAxis = i;
                        selectedSign = static_cast<float>(sign);
                    }
                }
            }
        }

        if (dragging &&
            ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            const ImVec2 delta = ImGui::GetIO().MouseDelta;

            if (ignoreFirstDragDelta)
            {
                ignoreFirstDragDelta = false;
            }
            else if (delta.x != 0.0f || delta.y != 0.0f)
            {
                const bool wasMoved = moved;
                moved = true;

                if (!wasMoved && !cursorCaptured)
                {
                    CursorVisibility::SetCaptured(window, true);
                    cursorCaptured = true;
                    ignoreFirstDragDelta = true;
                }

                const Vec3 offset = {
                    camera.position[0] - target.x,
                    camera.position[1] - target.y,
                    camera.position[2] - target.z
                };

                const float distance = std::max(
                    std::sqrt(Dot(offset, offset)),
                    0.1f
                );

                float yaw = std::atan2(offset.z, offset.x);

                float pitch = std::asin(std::clamp(
                    offset.y / distance,
                    -1.0f,
                    1.0f
                ));

                yaw += delta.x * sensitivity;
                pitch += delta.y * sensitivity;

                pitch = std::clamp(
                    pitch,
                    -Pi * 0.495f,
                    Pi * 0.495f
                );

                const float horizontal =
                    distance * std::cos(pitch);

                camera.position[0] =
                    target.x + horizontal * std::cos(yaw);

                camera.position[1] =
                    target.y + distance * std::sin(pitch);

                camera.position[2] =
                    target.z + horizontal * std::sin(yaw);

                camera.up[0] = 0.0f;
                camera.up[1] = 1.0f;
                camera.up[2] = 0.0f;
            }

            lastMousePosition = mouse;
        }

        if (dragging &&
            ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            if (!moved && selectedAxis >= 0)
            {
                const Vec3 direction = {
                    Axes[selectedAxis].x * selectedSign,
                    Axes[selectedAxis].y * selectedSign,
                    Axes[selectedAxis].z * selectedSign
                };

                const Vec3 offset = {
                    camera.position[0] - target.x,
                    camera.position[1] - target.y,
                    camera.position[2] - target.z
                };

                const float distance = std::max(
                    std::sqrt(Dot(offset, offset)),
                    1.0f
                );

                const Vec3 targetPosition = {
                    target.x + direction.x * distance,
                    target.y + direction.y * distance,
                    target.z + direction.z * distance
                };

                Vec3 targetUp = { 0.0f, 1.0f, 0.0f };

                if (selectedAxis == 1)
                {
                    targetUp = {
                        0.0f,
                        0.0f,
                        selectedSign > 0.0f ? -1.0f : 1.0f
                    };
                }

                WriteArray(
                    transitionStartPosition,
                    ReadVector(camera.position)
                );

                WriteArray(
                    transitionTargetPosition,
                    targetPosition
                );

                WriteArray(
                    transitionStartUp,
                    ReadVector(camera.up)
                );

                WriteArray(
                    transitionTargetUp,
                    targetUp
                );

                const Vec3 startPosition = ReadVector(camera.position);

                const Vec3 transitionDelta = {
                    targetPosition.x - startPosition.x,
                    targetPosition.y - startPosition.y,
                    targetPosition.z - startPosition.z
                };

                const float transitionDistance = std::sqrt(
                    Dot(transitionDelta, transitionDelta)
                );

                if (transitionDistance > 0.001f)
                {
                    transitionElapsed = 0.0f;
                    transitioning = true;
                }
            }

            dragging = false;
            moved = false;
            ignoreFirstDragDelta = false;
            selectedAxis = -1;

            if (cursorCaptured)
            {
                CursorVisibility::SetCaptured(window, false);
                cursorCaptured = false;
            }
        }

        const ImU32 background = hovered && canInteract
            ? IM_COL32(34, 37, 44, 245)
            : BackgroundColor;

        drawList->AddCircleFilled(
            center,
            GizmoBackgroundRadius,
            background,
            40
        );

        drawList->AddCircle(
            center,
            GizmoBackgroundRadius + GizmoBorderThickness,
            BorderColor,
            40,
            1.2f
        );

        drawList->AddCircleFilled(
            center,
            2.5f,
            IM_COL32(205, 210, 220, 210),
            16
        );

        GetCameraBasis(camera, forward, right, up);

        for (int i = 0; i < 3; ++i)
        {
            for (int sign : { -1, 1 })
            {
                const Vec3 direction = {
                    Axes[i].x * sign,
                    Axes[i].y * sign,
                    Axes[i].z * sign
                };

                const ImVec2 endpoint = ProjectAxis(
                    direction,
                    right,
                    up,
                    center,
                    GizmoRadius
                );

                const bool facingCamera =
                    Dot(direction, forward) < 0.0f;

                const ImU32 color = facingCamera
                    ? AxisColors[i]
                    : DimColor(AxisColors[i]);

                const float dotRadius = facingCamera
                    ? AxisDotRadius
                    : BackAxisDotRadius;

                drawList->AddLine(
                    center,
                    endpoint,
                    color,
                    facingCamera ? 3.0f : 2.0f
                );

                drawList->AddCircleFilled(
                    endpoint,
                    dotRadius + 1.2f,
                    IM_COL32(22, 24, 29, 235),
                    24
                );

                drawList->AddCircleFilled(
                    endpoint,
                    dotRadius,
                    color,
                    24
                );

                if (sign > 0)
                {
                    const char* label = AxisLabels[i];
                    const ImVec2 textSize = ImGui::CalcTextSize(label);

                    const ImVec2 textPosition(
                        endpoint.x - textSize.x * 0.5f,
                        endpoint.y - textSize.y * 0.5f
                    );

                    drawList->AddText(
                        textPosition,
                        TextColor,
                        label
                    );
                }
            }
        }
    }
}
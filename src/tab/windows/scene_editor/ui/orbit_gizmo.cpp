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

        struct Vec3
        {
            float x;
            float y;
            float z;
        };

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
            const float x = a.x - b.x;
            const float y = a.y - b.y;

            return x * x + y * y;
        }

        bool IsInside(
            const ImVec2& point,
            const ImVec2& center,
            float radius
        )
        {
            return DistanceSquared(point, center)
                <= radius * radius;
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

            const Vec3 offset = {
                camera.position[0] - target.x,
                camera.position[1] - target.y,
                camera.position[2] - target.z
            };

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

            // Hindari up vector sejajar dengan arah pandang.
            if (std::abs(direction.y) > 0.99f)
            {
                camera.up[0] = 0.0f;
                camera.up[1] = 0.0f;
                camera.up[2] =
                    direction.y > 0.0f ? -1.0f : 1.0f;
            }
            else
            {
                camera.up[0] = 0.0f;
                camera.up[1] = 1.0f;
                camera.up[2] = 0.0f;
            }
        }
    }

    void OrbitGizmo::Draw(
        SceneCamera& camera,
        const ImVec2& viewportMin,
        const ImVec2& viewportMax
    )
    {
        constexpr float gizmoRadius = 34.0f;
        constexpr float hitRadius = 12.0f;
        constexpr float interactionRadius = 42.0f;

        GLFWwindow* window = glfwGetCurrentContext();

        const ImVec2 center(
            viewportMax.x - 52.0f,
            viewportMin.y + 52.0f
        );

        // Pulihkan kursor jika viewport menjadi terlalu kecil.
        if (center.x - interactionRadius < viewportMin.x ||
            center.y + interactionRadius > viewportMax.y)
        {
            if (dragging)
            {
                CursorVisibility::SetCaptured(window, false);
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

        const Vec3 offset = {
            camera.position[0] - target.x,
            camera.position[1] - target.y,
            camera.position[2] - target.z
        };

        const float distance = std::max(
            std::sqrt(Dot(offset, offset)),
            0.1f
        );

        const Vec3 forward = Normalize({
            -offset.x,
            -offset.y,
            -offset.z
            });

        const Vec3 worldUp = {
            0.0f,
            1.0f,
            0.0f
        };

        Vec3 right = Normalize(Cross(forward, worldUp));

        if (Dot(right, right) < 0.0001f)
        {
            right = { 1.0f, 0.0f, 0.0f };
        }

        const Vec3 up = Normalize(Cross(right, forward));

        const std::array<Vec3, 3> axes = { {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
        } };

        const std::array<ImU32, 3> colors = { {
            IM_COL32(235, 75, 75, 255),
            IM_COL32(95, 205, 115, 255),
            IM_COL32(80, 145, 255, 255)
        } };

        const char* labels[] = {
            "X",
            "Y",
            "Z"
        };

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        drawList->AddCircleFilled(
            center,
            40.0f,
            IM_COL32(25, 27, 31, 225),
            32
        );

        drawList->AddCircle(
            center,
            40.0f,
            IM_COL32(90, 95, 105, 220),
            32,
            1.0f
        );

        const ImVec2 mouse = ImGui::GetIO().MousePos;

        const bool hovered = IsInside(
            mouse,
            center,
            interactionRadius
        );

        // Mulai drag ketika tombol kiri ditekan di area gizmo.
        if (!dragging &&
            hovered &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            dragging = true;
            moved = false;
            ignoreFirstDragDelta = true;

            lastMousePosition = mouse;
            selectedAxis = -1;

            float closestDistance = hitRadius * hitRadius;

            // Tentukan sumbu untuk aksi klik tanpa drag.
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

                    const float d = DistanceSquared(
                        mouse,
                        endpoint
                    );

                    if (d < closestDistance)
                    {
                        closestDistance = d;
                        selectedAxis = i;
                        selectedSign = static_cast<float>(sign);
                    }
                }
            }

            // Sembunyikan dan tangkap kursor selama drag.
            CursorVisibility::SetCaptured(window, true);
        }

        // Orbit selama tombol kiri ditahan.
        if (dragging &&
            ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            const ImVec2 delta = ImGui::GetIO().MouseDelta;

            // Abaikan delta awal agar kamera tidak meloncat.
            if (ignoreFirstDragDelta)
            {
                ignoreFirstDragDelta = false;
            }
            else if (delta.x != 0.0f || delta.y != 0.0f)
            {
                moved = true;

                float yaw = std::atan2(
                    offset.z,
                    offset.x
                );

                float pitch = std::asin(
                    std::clamp(
                        offset.y / distance,
                        -1.0f,
                        1.0f
                    )
                );

                // Samakan arah drag dengan OrbitNavigation.
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

        // Lepas drag dan pulihkan kursor.
        if (dragging &&
            ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            // Klik sumbu tanpa menggeser mouse akan snap ke sumbu.
            if (!moved && selectedAxis >= 0)
            {
                const Vec3 direction = {
                    axes[selectedAxis].x * selectedSign,
                    axes[selectedAxis].y * selectedSign,
                    axes[selectedAxis].z * selectedSign
                };

                SnapToAxis(camera, direction);
            }

            dragging = false;
            moved = false;
            ignoreFirstDragDelta = false;
            selectedAxis = -1;

            CursorVisibility::SetCaptured(window, false);
        }

        // Hitung ulang orientasi gizmo berdasarkan posisi kamera terbaru.
        const Vec3 newOffset = {
            camera.position[0] - target.x,
            camera.position[1] - target.y,
            camera.position[2] - target.z
        };

        const Vec3 newForward = Normalize({
            -newOffset.x,
            -newOffset.y,
            -newOffset.z
            });

        Vec3 newRight = Normalize(
            Cross(newForward, worldUp)
        );

        if (Dot(newRight, newRight) < 0.0001f)
        {
            newRight = { 1.0f, 0.0f, 0.0f };
        }

        const Vec3 newUp = Normalize(
            Cross(newRight, newForward)
        );

        // Gambar sumbu positif dan negatif.
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
                    newRight,
                    newUp,
                    center,
                    gizmoRadius
                );

                const bool front =
                    Dot(direction, newForward) < 0.0f;

                const ImU32 color = front
                    ? colors[i]
                    : IM_COL32(100, 105, 115, 180);

                drawList->AddLine(
                    center,
                    endpoint,
                    color,
                    front ? 3.0f : 2.0f
                );

                drawList->AddCircleFilled(
                    endpoint,
                    front ? 7.0f : 5.0f,
                    color,
                    20
                );

                drawList->AddText(
                    ImVec2(
                        endpoint.x - 4.0f,
                        endpoint.y - 7.0f
                    ),
                    IM_COL32(255, 255, 255, 255),
                    labels[i]
                );
            }
        }
    }
}

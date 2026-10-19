
#include "tab/windows/scene_editor/navigation/orbit_navigation.h"

#include "core/input/cursor_visibility.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    void OrbitNavigation::Update(
        SceneCamera& camera,
        GLFWwindow* window,
        bool viewportHovered
    )
    {
        constexpr int orbitButton = ImGuiMouseButton_Middle;
        constexpr float minRadius = 0.1f;
        constexpr float minPitch = -1.55334306f;
        constexpr float maxPitch = 1.55334306f;
        constexpr int startupFramesToIgnore = 2;

        // Mulai orbit hanya jika klik terjadi di viewport.
        if (ImGui::IsMouseClicked(orbitButton) &&
            viewportHovered &&
            !orbiting)
        {
            orbiting = true;

            // Hindari delta mouse akibat pergantian mode kursor.
            ignoreMouseDeltaFrames = startupFramesToIgnore;

            CursorVisibility::SetCaptured(
                window,
                true
            );

            // Jangan memutar kamera pada frame klik awal.
            return;
        }

        // Pulihkan kursor ketika tombol tengah dilepas.
        if (!ImGui::IsMouseDown(orbitButton))
        {
            if (orbiting)
            {
                CursorVisibility::SetCaptured(
                    window,
                    false
                );
            }

            orbiting = false;
            ignoreMouseDeltaFrames = 0;
            return;
        }

        if (!orbiting)
            return;

        // Abaikan frame awal setelah kursor ditangkap.
        if (ignoreMouseDeltaFrames > 0)
        {
            --ignoreMouseDeltaFrames;
            return;
        }

        const ImGuiIO& io = ImGui::GetIO();

        const float deltaX = io.MouseDelta.x;
        const float deltaY = io.MouseDelta.y;

        // Tidak ada gerakan mouse, tidak perlu menghitung ulang kamera.
        if (deltaX == 0.0f && deltaY == 0.0f)
            return;

        // Posisi kamera relatif terhadap target orbit.
        const float offsetX =
            camera.position[0] - camera.target[0];

        const float offsetY =
            camera.position[1] - camera.target[1];

        const float offsetZ =
            camera.position[2] - camera.target[2];

        const float radius = std::sqrt(
            offsetX * offsetX +
            offsetY * offsetY +
            offsetZ * offsetZ
        );

        if (radius < minRadius)
            return;

        float yaw = std::atan2(
            offsetZ,
            offsetX
        );

        float pitch = std::asin(
            std::clamp(
                offsetY / radius,
                -1.0f,
                1.0f
            )
        );

        // Rotasi horizontal.
        yaw += deltaX * sensitivity;

        // Rotasi vertikal.
        pitch += deltaY * sensitivity;

        pitch = std::clamp(
            pitch,
            minPitch,
            maxPitch
        );

        const float horizontalRadius =
            radius * std::cos(pitch);

        camera.position[0] =
            camera.target[0] +
            horizontalRadius * std::cos(yaw);

        camera.position[1] =
            camera.target[1] +
            radius * std::sin(pitch);

        camera.position[2] =
            camera.target[2] +
            horizontalRadius * std::sin(yaw);
    }
}

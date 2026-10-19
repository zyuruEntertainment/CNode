#include "tab/windows/scene_editor/navigation/orbit_navigation.h"

#include "imgui.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    void OrbitNavigation::Update(
        SceneCamera& camera,
        bool viewportHovered
    )
    {
        constexpr int orbitButton = ImGuiMouseButton_Middle;
        constexpr float minRadius = 0.1f;
        constexpr float minPitch = -1.55334306f;
        constexpr float maxPitch = 1.55334306f;
        constexpr float pi = 3.14159265358979323846f;

        // Orbit hanya boleh dimulai dari viewport 3D.
        if (ImGui::IsMouseClicked(orbitButton) &&
            viewportHovered)
        {
            orbiting = true;
        }

        // Berhenti saat tombol tengah dilepas.
        if (!ImGui::IsMouseDown(orbitButton))
        {
            orbiting = false;
            return;
        }

        if (!orbiting)
            return;

        const ImGuiIO& io = ImGui::GetIO();

        // Offset kamera relatif terhadap titik target.
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

        // Konversi posisi kamera menjadi sudut orbit.
        float yaw = std::atan2(offsetZ, offsetX);

        float pitch = std::asin(
            std::clamp(
                offsetY / radius,
                -1.0f,
                1.0f
            )
        );

        // Mouse horizontal mengubah yaw.
        yaw -= io.MouseDelta.x * sensitivity;

        // Mouse vertikal mengubah pitch.
        pitch -= io.MouseDelta.y * sensitivity;

        pitch = std::clamp(
            pitch,
            minPitch,
            maxPitch
        );

        // Kembalikan sudut menjadi posisi kamera.
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
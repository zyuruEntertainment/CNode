#pragma once

namespace CNode
{
    class SceneCamera
    {
    public:
        SceneCamera() = default;

        void GetViewProjection(
            float width,
            float height,
            float* output
        ) const;

        float position[3] = { 8.0f, 7.0f, 8.0f };
        float target[3] = { 0.0f, 0.0f, 0.0f };
        float up[3] = { 0.0f, 1.0f, 0.0f };

        float fieldOfView = 60.0f;
        float nearClip = 0.1f;
        float farClip = 1000.0f;
    };
}

#include "tab/windows/scene_editor/scene_camera.h"

#include <algorithm>
#include <cmath>

namespace CNode
{
    namespace
    {
        constexpr float Epsilon = 0.00001f;

        struct Vec3
        {
            float x;
            float y;
            float z;
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
            return
                a.x * b.x +
                a.y * b.y +
                a.z * b.z;
        }

        float Length(const Vec3& value)
        {
            return std::sqrt(Dot(value, value));
        }

        Vec3 Normalize(const Vec3& value)
        {
            const float length = Length(value);

            if (length <= Epsilon)
                return { 0.0f, 0.0f, 0.0f };

            return {
                value.x / length,
                value.y / length,
                value.z / length
            };
        }

        void Identity(float* matrix)
        {
            for (int i = 0; i < 16; ++i)
                matrix[i] = 0.0f;

            matrix[0] = 1.0f;
            matrix[5] = 1.0f;
            matrix[10] = 1.0f;
            matrix[15] = 1.0f;
        }

        void Multiply(
            const float* a,
            const float* b,
            float* output
        )
        {
            float result[16] = {};

            for (int column = 0; column < 4; ++column)
            {
                for (int row = 0; row < 4; ++row)
                {
                    for (int k = 0; k < 4; ++k)
                    {
                        result[column * 4 + row] +=
                            a[k * 4 + row] *
                            b[column * 4 + k];
                    }
                }
            }

            for (int i = 0; i < 16; ++i)
                output[i] = result[i];
        }

        void LookAt(
            const Vec3& eye,
            const Vec3& target,
            const Vec3& requestedUp,
            float* output
        )
        {
            Vec3 forward = Normalize(Subtract(target, eye));

            if (Length(forward) <= Epsilon)
                forward = { 0.0f, 0.0f, -1.0f };

            Vec3 upReference = Normalize(requestedUp);

            if (Length(upReference) <= Epsilon)
                upReference = { 0.0f, 1.0f, 0.0f };

            Vec3 right = Normalize(Cross(forward, upReference));

            if (Length(right) <= Epsilon)
            {
                upReference =
                    std::abs(forward.y) > 0.99f
                    ? Vec3{ 0.0f, 0.0f, 1.0f }
                : Vec3{ 0.0f, 1.0f, 0.0f };

                right = Normalize(Cross(forward, upReference));
            }

            Vec3 cameraUp = Normalize(Cross(right, forward));

            if (Length(right) <= Epsilon ||
                Length(cameraUp) <= Epsilon)
            {
                right = { 1.0f, 0.0f, 0.0f };
                cameraUp = Normalize(Cross(right, forward));
            }

            Identity(output);

            output[0] = right.x;
            output[1] = cameraUp.x;
            output[2] = -forward.x;

            output[4] = right.y;
            output[5] = cameraUp.y;
            output[6] = -forward.y;

            output[8] = right.z;
            output[9] = cameraUp.z;
            output[10] = -forward.z;

            output[12] = -Dot(right, eye);
            output[13] = -Dot(cameraUp, eye);
            output[14] = Dot(forward, eye);
        }

        void Perspective(
            float fieldOfView,
            float aspectRatio,
            float nearClip,
            float farClip,
            float* output
        )
        {
            for (int i = 0; i < 16; ++i)
                output[i] = 0.0f;

            constexpr float Pi = 3.14159265358979323846f;

            const float radians = fieldOfView * Pi / 180.0f;
            const float scale = 1.0f / std::tan(radians * 0.5f);

            output[0] = scale / aspectRatio;
            output[5] = scale;
            output[10] =
                (farClip + nearClip) /
                (nearClip - farClip);

            output[11] = -1.0f;

            output[14] =
                (2.0f * farClip * nearClip) /
                (nearClip - farClip);
        }
    }

    void SceneCamera::GetViewProjection(
        float width,
        float height,
        float* output
    ) const
    {
        if (!output)
            return;

        width = std::max(width, 1.0f);
        height = std::max(height, 1.0f);

        const Vec3 eye = {
            position[0],
            position[1],
            position[2]
        };

        const Vec3 lookTarget = {
            target[0],
            target[1],
            target[2]
        };

        const Vec3 worldUp = {
            up[0],
            up[1],
            up[2]
        };

        float view[16];
        float projection[16];

        LookAt(eye, lookTarget, worldUp, view);

        Perspective(
            fieldOfView,
            width / height,
            nearClip,
            farClip,
            projection
        );

        Multiply(projection, view, output);
    }
}

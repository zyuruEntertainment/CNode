#pragma once

namespace CNode
{
    struct Vec3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
    };

    struct TransformComponent
    {
        Vec3 position{};
        Vec3 rotation{};
        Vec3 scale{ 1.0f, 1.0f, 1.0f };
    };
}
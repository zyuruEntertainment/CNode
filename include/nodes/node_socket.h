#pragma once

#include <string>

namespace CNode
{
    enum class SocketDirection
    {
        Input,
        Output
    };

    enum class SocketType
    {
        Transform,
        Mesh,
        Renderer,
        Float,
        Vector3
    };

    struct NodeSocket
    {
        unsigned long long id = 0;
        unsigned long long nodeId = 0;
        std::string name;
        SocketDirection direction = SocketDirection::Output;
        SocketType type = SocketType::Mesh;

        float x = 0.0f;
        float y = 0.0f;
    };
}
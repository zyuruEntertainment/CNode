#pragma once

#include "nodes/node_socket.h"

#include <string>
#include <vector>

namespace CNode
{
    enum class NodeType
    {
        Cube
    };

    struct Node
    {
        unsigned long long id = 0;
        std::string type = "cube";
        std::string label = "Cube";
        float x = 100.0f;
        float y = 100.0f;

        std::vector<NodeSocket> sockets;
        bool selected = false;
    };
}
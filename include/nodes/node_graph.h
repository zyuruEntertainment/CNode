#pragma once

#include "nodes/node.h"
#include "nodes/node_connection.h"

#include <vector>

namespace CNode
{
    struct NodeGraph
    {
        std::vector<Node> nodes;
        std::vector<NodeConnection> connections;
        unsigned long long selectedNodeId = 0;
        unsigned long long nextNodeId = 1;
        unsigned long long nextSocketId = 1;
        unsigned long long nextConnectionId = 1;
    };
}
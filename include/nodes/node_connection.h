#pragma once

namespace CNode
{
    struct NodeConnection
    {
        unsigned long long id = 0;
        unsigned long long fromNodeId = 0;
        unsigned long long fromSocketId = 0;
        unsigned long long toNodeId = 0;
        unsigned long long toSocketId = 0;
    };
}
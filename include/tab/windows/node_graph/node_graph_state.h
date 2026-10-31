#pragma once

#include "nodes/node.h"
#include "nodes/node_socket.h"

#include "imgui.h"

namespace CNode
{
    struct NodeGraphState
    {
        bool dragging = false;
        unsigned long long dragNodeId = 0;
        float dragOffsetX = 0.0f;
        float dragOffsetY = 0.0f;

        bool connecting = false;
        unsigned long long connectFromNodeId = 0;
        unsigned long long connectFromSocketId = 0;
    };

    struct NodeGraphColors
    {
        ImU32 nodeColor = IM_COL32(45, 45, 50, 255);
        ImU32 selectedColor = IM_COL32(70, 90, 130, 255);
        ImU32 headerColor = IM_COL32(30, 30, 35, 255);
        ImU32 textColor = IM_COL32(220, 220, 220, 255);
        ImU32 inputSocketColor = IM_COL32(120, 180, 255, 255);
        ImU32 outputSocketColor = IM_COL32(255, 200, 100, 255);
        ImU32 connectionColor = IM_COL32(180, 180, 180, 255);
        ImU32 invalidColor = IM_COL32(220, 80, 80, 255);
    };

    struct NodeGraphHitResult
    {
        bool nodeHit = false;
        bool interactiveHovered = false;
        NodeSocket* clickedSocket = nullptr;
        Node* clickedNode = nullptr;
        bool clickedOutput = false;
    };
}

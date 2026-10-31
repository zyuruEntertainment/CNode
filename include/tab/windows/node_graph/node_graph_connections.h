#pragma once

#include "tab/windows/node_graph/node_graph_state.h"

#include "scene/scene.h"

#include "imgui.h"

namespace CNode
{
    Node* FindNode(NodeGraph& graph, unsigned long long id);
    NodeSocket* FindSocket(Node& node, unsigned long long id);
    bool CanConnect(
        SceneObject& object,
        Node& fromNode,
        NodeSocket& fromSocket,
        Node& toNode,
        NodeSocket& toSocket
    );

    void DrawConnections(
        SceneObject& object,
        ImDrawList* drawList,
        const NodeGraphColors& colors
    );

    void DrawConnectionPreview(
        const NodeGraphState& state,
        SceneObject& object,
        ImDrawList* drawList,
        ImVec2 mousePos,
        const NodeGraphColors& colors
    );
}

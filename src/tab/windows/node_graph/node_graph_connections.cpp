#include "tab/windows/node_graph/node_graph_connections.h"

#include <cmath>

namespace CNode
{
    Node* FindNode(NodeGraph& graph, unsigned long long id)
    {
        for (Node& node : graph.nodes)
        {
            if (node.id == id)
                return &node;
        }

        return nullptr;
    }

    NodeSocket* FindSocket(Node& node, unsigned long long id)
    {
        for (NodeSocket& socket : node.sockets)
        {
            if (socket.id == id)
                return &socket;
        }

        return nullptr;
    }

    bool SocketTypeCompatible(SocketType a, SocketType b)
    {
        return a == b;
    }

    bool CanConnect(
        SceneObject& object,
        Node& fromNode,
        NodeSocket& fromSocket,
        Node& toNode,
        NodeSocket& toSocket
    )
    {
        if (fromSocket.direction != SocketDirection::Output)
            return false;

        if (toSocket.direction != SocketDirection::Input)
            return false;

        if (fromNode.id == toNode.id)
            return false;

        if (!SocketTypeCompatible(fromSocket.type, toSocket.type))
            return false;

        for (const NodeConnection& connection : object.graph.connections)
        {
            if (connection.toNodeId == toNode.id &&
                connection.toSocketId == toSocket.id)
            {
                return false;
            }
        }

        return true;
    }

    void DrawConnections(
        SceneObject& object,
        ImDrawList* drawList,
        const NodeGraphColors& colors
    )
    {
        for (const NodeConnection& connection : object.graph.connections)
        {
            Node* fromNode = FindNode(object.graph, connection.fromNodeId);
            Node* toNode = FindNode(object.graph, connection.toNodeId);

            if (!fromNode || !toNode)
                continue;

            NodeSocket* fromSocket = FindSocket(*fromNode, connection.fromSocketId);
            NodeSocket* toSocket = FindSocket(*toNode, connection.toSocketId);

            if (!fromSocket || !toSocket)
                continue;

            drawList->AddLine(
                ImVec2(fromSocket->x, fromSocket->y),
                ImVec2(toSocket->x, toSocket->y),
                colors.connectionColor,
                2.0f
            );
        }
    }

    void DrawConnectionPreview(
        const NodeGraphState& state,
        SceneObject& object,
        ImDrawList* drawList,
        ImVec2 mousePos,
        const NodeGraphColors& colors
    )
    {
        if (!state.connecting)
            return;

        Node* fromNode = FindNode(object.graph, state.connectFromNodeId);
        NodeSocket* fromSocket = fromNode ? FindSocket(*fromNode, state.connectFromSocketId) : nullptr;

        if (!fromSocket)
            return;

        ImU32 color = colors.connectionColor;
        Node* hoveredNode = nullptr;
        NodeSocket* hoveredSocket = nullptr;

        for (Node& node : object.graph.nodes)
        {
            for (NodeSocket& socket : node.sockets)
            {
                if (socket.direction != SocketDirection::Input)
                    continue;

                float distance = std::sqrt(
                    (mousePos.x - socket.x) * (mousePos.x - socket.x) +
                    (mousePos.y - socket.y) * (mousePos.y - socket.y)
                );

                if (distance <= 8.0f)
                {
                    hoveredNode = &node;
                    hoveredSocket = &socket;
                }
            }
        }

        if (hoveredSocket)
        {
            bool valid = hoveredNode && CanConnect(
                object,
                *fromNode,
                *fromSocket,
                *hoveredNode,
                *hoveredSocket
            );

            color = valid ? colors.connectionColor : colors.invalidColor;
        }

        drawList->AddLine(
            ImVec2(fromSocket->x, fromSocket->y),
            mousePos,
            color,
            2.0f
        );
    }
}

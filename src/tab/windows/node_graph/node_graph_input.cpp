#include "tab/windows/node_graph/node_graph_input.h"
#include "tab/windows/node_graph/node_graph_connections.h"

namespace CNode
{
    void HandleNodeGraphInput(
        NodeGraphState& state,
        SceneObject& object,
        ImVec2 canvasOrigin,
        ImVec2 mousePos,
        const NodeGraphHitResult& hit
    )
    {
        if (state.dragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            state.dragging = false;
            state.dragNodeId = 0;
        }

        if (hit.clickedSocket && hit.clickedNode)
        {
            if (hit.clickedOutput)
            {
                state.connecting = true;
                state.connectFromNodeId = hit.clickedNode->id;
                state.connectFromSocketId = hit.clickedSocket->id;
            }
            else if (state.connecting)
            {
                Node* fromNode = FindNode(object.graph, state.connectFromNodeId);
                NodeSocket* fromSocket = fromNode ? FindSocket(*fromNode, state.connectFromSocketId) : nullptr;

                if (fromNode && fromSocket && CanConnect(
                        object,
                        *fromNode,
                        *fromSocket,
                        *hit.clickedNode,
                        *hit.clickedSocket
                    ))
                {
                    NodeConnection connection;
                    connection.id = object.graph.nextConnectionId++;
                    connection.fromNodeId = fromNode->id;
                    connection.fromSocketId = fromSocket->id;
                    connection.toNodeId = hit.clickedNode->id;
                    connection.toSocketId = hit.clickedSocket->id;

                    object.graph.connections.push_back(connection);
                }

                state.connecting = false;
                state.connectFromNodeId = 0;
                state.connectFromSocketId = 0;
            }
        }

        if (state.connecting && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            state.connecting = false;
            state.connectFromNodeId = 0;
            state.connectFromSocketId = 0;
        }
    }
}

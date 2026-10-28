#include "tab/windows/node_graph/node_graph.h"
#include "scene/scene.h"

#include "imgui.h"

#include <cmath>
#include <string>
#include <vector>

namespace CNode
{
    namespace
    {
        bool gDragging = false;
        unsigned long long gDragNodeId = 0;
        float gDragOffsetX = 0.0f;
        float gDragOffsetY = 0.0f;

        bool gConnecting = false;
        unsigned long long gConnectFromNodeId = 0;
        unsigned long long gConnectFromSocketId = 0;
    }

    static Node* FindNode(NodeGraph& graph, unsigned long long id)
    {
        for (Node& node : graph.nodes)
        {
            if (node.id == id)
                return &node;
        }

        return nullptr;
    }

    static NodeSocket* FindSocket(Node& node, unsigned long long id)
    {
        for (NodeSocket& socket : node.sockets)
        {
            if (socket.id == id)
                return &socket;
        }

        return nullptr;
    }

    static bool SocketTypeCompatible(SocketType a, SocketType b)
    {
        return a == b;
    }

    static bool CanConnect(
        SceneObject& object,
        Node& fromNode,
        NodeSocket& fromSocket,
        Node& toNode,
        NodeSocket& toSocket)
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

    void DrawNodeGraph(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        Scene& scene = GetScene();
        SceneObject* object = GetSelectedSceneObject();

        if (!object)
        {
            ImGui::TextUnformatted("No object selected.");
            ImGui::TextUnformatted("Create or select an object in Object List.");
            ImGui::End();
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.26f, 0.30f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.36f, 0.36f, 0.40f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.46f, 0.46f, 0.50f, 1.0f));
        if (ImGui::Button("+"))
            ImGui::OpenPopup("AddNodePopup");
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);

        if (ImGui::BeginPopup("AddNodePopup"))
        {
            if (ImGui::MenuItem("Cube"))
            {
                CreateCubeNode(*object);
            }

            ImGui::EndPopup();
        }

        ImGui::Separator();

        ImGui::BeginChild("NodeCanvas", ImVec2(0, 0), true);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();
        ImVec2 mousePos = ImGui::GetMousePos();

        ImU32 nodeColor = IM_COL32(45, 45, 50, 255);
        ImU32 selectedColor = IM_COL32(70, 90, 130, 255);
        ImU32 headerColor = IM_COL32(30, 30, 35, 255);
        ImU32 textColor = IM_COL32(220, 220, 220, 255);
        ImU32 inputSocketColor = IM_COL32(120, 180, 255, 255);
        ImU32 outputSocketColor = IM_COL32(255, 200, 100, 255);
        ImU32 connectionColor = IM_COL32(180, 180, 180, 255);
        ImU32 invalidColor = IM_COL32(220, 80, 80, 255);

        if (object->graph.nodes.empty())
        {
            ImGui::SetCursorPos(ImVec2(20, 30));
            ImGui::TextUnformatted("Empty graph. Use + to add a Cube.");
        }

        // Draw existing connections.
        for (const NodeConnection& connection : object->graph.connections)
        {
            Node* fromNode = FindNode(object->graph, connection.fromNodeId);
            Node* toNode = FindNode(object->graph, connection.toNodeId);

            if (!fromNode || !toNode)
                continue;

            NodeSocket* fromSocket = FindSocket(*fromNode, connection.fromSocketId);
            NodeSocket* toSocket = FindSocket(*toNode, connection.toSocketId);

            if (!fromSocket || !toSocket)
                continue;

            drawList->AddLine(
                ImVec2(fromSocket->x, fromSocket->y),
                ImVec2(toSocket->x, toSocket->y),
                connectionColor,
                2.0f
            );
        }

        // Draw connection preview.
        if (gConnecting)
        {
            Node* fromNode = FindNode(object->graph, gConnectFromNodeId);
            NodeSocket* fromSocket = fromNode ? FindSocket(*fromNode, gConnectFromSocketId) : nullptr;

            if (fromSocket)
            {
                ImU32 color = connectionColor;
                Node* hoveredNode = nullptr;
                NodeSocket* hoveredSocket = nullptr;

                for (Node& node : object->graph.nodes)
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
                        *object,
                        *fromNode,
                        *fromSocket,
                        *hoveredNode,
                        *hoveredSocket
                    );

                    color = valid ? connectionColor : invalidColor;
                }

                drawList->AddLine(
                    ImVec2(fromSocket->x, fromSocket->y),
                    mousePos,
                    color,
                    2.0f
                );
            }
        }

        bool nodeHit = false;
        bool interactiveHovered = false;
        NodeSocket* clickedSocket = nullptr;
        Node* clickedNode = nullptr;
        bool clickedOutput = false;

        for (Node& node : object->graph.nodes)
        {
            int inputCount = 0;
            int outputCount = 0;

            for (const NodeSocket& socket : node.sockets)
            {
                if (socket.direction == SocketDirection::Input)
                    ++inputCount;
                else
                    ++outputCount;
            }

            int rowCount = std::max(1, std::max(inputCount, outputCount));

            float maxLabelWidth = 120.0f;

            for (const NodeSocket& socket : node.sockets)
            {
                ImVec2 textSize = ImGui::CalcTextSize(socket.name.c_str());
                maxLabelWidth = std::max(maxLabelWidth, textSize.x + 34.0f);
            }

            ImVec2 size(
                std::max(230.0f, maxLabelWidth),
                58.0f + static_cast<float>(rowCount) * 18.0f
            );

            ImVec2 pos(canvasOrigin.x + node.x, canvasOrigin.y + node.y);
            ImVec2 max(pos.x + size.x, pos.y + size.y);

            bool hovered = (mousePos.x >= pos.x && mousePos.x <= max.x &&
                            mousePos.y >= pos.y && mousePos.y <= max.y);

            ImU32 fill = (node.id == object->graph.selectedNodeId)
                ? selectedColor : nodeColor;

            drawList->AddRectFilled(pos, max, fill, 6.0f);
            drawList->AddRectFilled(pos, ImVec2(max.x, pos.y + 24.0f), headerColor, 6.0f, ImDrawFlags_RoundCornersTop);
            drawList->AddRect(pos, max, IM_COL32(90, 90, 95, 255), 6.0f);
            drawList->AddText(ImVec2(pos.x + 8.0f, pos.y + 4.0f), textColor, node.label.c_str());

            drawList->AddText(ImVec2(pos.x + 8.0f, pos.y + 28.0f), textColor, node.type.c_str());

            int inputIndex = 0;
            int outputIndex = 0;

            for (NodeSocket& socket : node.sockets)
            {
                if (socket.direction == SocketDirection::Input)
                {
                    socket.x = pos.x;
                    socket.y = pos.y + 46.0f + static_cast<float>(inputIndex) * 18.0f;

                    drawList->AddCircleFilled(ImVec2(socket.x, socket.y), 5.0f, inputSocketColor);
                    drawList->AddText(ImVec2(pos.x + 10.0f, socket.y - 6.0f), textColor, socket.name.c_str());

                    float distance = std::sqrt(
                        (mousePos.x - socket.x) * (mousePos.x - socket.x) +
                        (mousePos.y - socket.y) * (mousePos.y - socket.y)
                    );

                    if (distance <= 8.0f)
                        interactiveHovered = true;

                    if (distance <= 8.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        clickedNode = &node;
                        clickedSocket = &socket;
                    }

                    ++inputIndex;
                }
                else
                {
                    socket.x = pos.x + size.x;
                    socket.y = pos.y + 46.0f + static_cast<float>(outputIndex) * 18.0f;

                    drawList->AddCircleFilled(ImVec2(socket.x, socket.y), 5.0f, outputSocketColor);

                    ImVec2 textSize = ImGui::CalcTextSize(socket.name.c_str());
                    drawList->AddText(
                        ImVec2(socket.x - textSize.x - 10.0f, socket.y - 6.0f),
                        textColor,
                        socket.name.c_str()
                    );

                    float distance = std::sqrt(
                        (mousePos.x - socket.x) * (mousePos.x - socket.x) +
                        (mousePos.y - socket.y) * (mousePos.y - socket.y)
                    );

                    if (distance <= 8.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        clickedNode = &node;
                        clickedSocket = &socket;
                        clickedOutput = true;
                    }

                    ++outputIndex;
                }
            }

            if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !clickedSocket)
            {
                object->graph.selectedNodeId = node.id;

                gDragging = true;
                gDragNodeId = node.id;
                gDragOffsetX = mousePos.x - pos.x;
                gDragOffsetY = mousePos.y - pos.y;
                nodeHit = true;
            }

            if (gDragging && gDragNodeId == node.id && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                node.x = mousePos.x - canvasOrigin.x - gDragOffsetX;
                node.y = mousePos.y - canvasOrigin.y - gDragOffsetY;
                nodeHit = true;
            }

            if (hovered)
            {
                nodeHit = true;
                interactiveHovered = true;
            }
        }

        if (gDragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            gDragging = false;
            gDragNodeId = 0;
        }

        if (clickedSocket && clickedNode)
        {
            if (clickedOutput)
            {
                gConnecting = true;
                gConnectFromNodeId = clickedNode->id;
                gConnectFromSocketId = clickedSocket->id;
            }
            else if (gConnecting)
            {
                Node* fromNode = FindNode(object->graph, gConnectFromNodeId);
                NodeSocket* fromSocket = fromNode ? FindSocket(*fromNode, gConnectFromSocketId) : nullptr;

                if (fromNode && fromSocket && CanConnect(*object, *fromNode, *fromSocket, *clickedNode, *clickedSocket))
                {
                    NodeConnection connection;
                    connection.id = object->graph.nextConnectionId++;
                    connection.fromNodeId = fromNode->id;
                    connection.fromSocketId = fromSocket->id;
                    connection.toNodeId = clickedNode->id;
                    connection.toSocketId = clickedSocket->id;

                    object->graph.connections.push_back(connection);
                }

                gConnecting = false;
                gConnectFromNodeId = 0;
                gConnectFromSocketId = 0;
            }
        }

        if (gConnecting && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            gConnecting = false;
            gConnectFromNodeId = 0;
            gConnectFromSocketId = 0;
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !interactiveHovered)
            ImGui::OpenPopup("AddNodePopup");

        if (!gDragging && !gConnecting && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !nodeHit && !clickedSocket)
        {
            object->graph.selectedNodeId = 0;
        }

        ImGui::EndChild();

        ImGui::End();
    }
}
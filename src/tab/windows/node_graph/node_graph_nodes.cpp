#include "tab/windows/node_graph/node_graph_nodes.h"

#include <cmath>
#include <algorithm>
#include <string>

namespace CNode
{
    void DrawNodes(
        NodeGraphState& state,
        SceneObject& object,
        ImDrawList* drawList,
        ImVec2 canvasOrigin,
        ImVec2 mousePos,
        const NodeGraphColors& colors,
        NodeGraphHitResult& hit
    )
    {
        for (Node& node : object.graph.nodes)
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

            ImU32 fill = (node.id == object.graph.selectedNodeId)
                ? colors.selectedColor : colors.nodeColor;

            drawList->AddRectFilled(pos, max, fill, 6.0f);
            drawList->AddRectFilled(
                pos,
                ImVec2(max.x, pos.y + 24.0f),
                colors.headerColor,
                6.0f,
                ImDrawFlags_RoundCornersTop
            );
            drawList->AddRect(pos, max, IM_COL32(90, 90, 95, 255), 6.0f);
            drawList->AddText(ImVec2(pos.x + 8.0f, pos.y + 4.0f), colors.textColor, node.label.c_str());
            drawList->AddText(ImVec2(pos.x + 8.0f, pos.y + 28.0f), colors.textColor, node.type.c_str());

            int inputIndex = 0;
            int outputIndex = 0;

            for (NodeSocket& socket : node.sockets)
            {
                if (socket.direction == SocketDirection::Input)
                {
                    socket.x = pos.x;
                    socket.y = pos.y + 46.0f + static_cast<float>(inputIndex) * 18.0f;

                    drawList->AddCircleFilled(ImVec2(socket.x, socket.y), 5.0f, colors.inputSocketColor);
                    drawList->AddText(ImVec2(pos.x + 10.0f, socket.y - 6.0f), colors.textColor, socket.name.c_str());

                    float distance = std::sqrt(
                        (mousePos.x - socket.x) * (mousePos.x - socket.x) +
                        (mousePos.y - socket.y) * (mousePos.y - socket.y)
                    );

                    if (distance <= 8.0f)
                        hit.interactiveHovered = true;

                    if (distance <= 8.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        hit.clickedNode = &node;
                        hit.clickedSocket = &socket;
                    }

                    ++inputIndex;
                }
                else
                {
                    socket.x = pos.x + size.x;
                    socket.y = pos.y + 46.0f + static_cast<float>(outputIndex) * 18.0f;

                    drawList->AddCircleFilled(ImVec2(socket.x, socket.y), 5.0f, colors.outputSocketColor);

                    ImVec2 textSize = ImGui::CalcTextSize(socket.name.c_str());
                    drawList->AddText(
                        ImVec2(socket.x - textSize.x - 10.0f, socket.y - 6.0f),
                        colors.textColor,
                        socket.name.c_str()
                    );

                    float distance = std::sqrt(
                        (mousePos.x - socket.x) * (mousePos.x - socket.x) +
                        (mousePos.y - socket.y) * (mousePos.y - socket.y)
                    );

                    if (distance <= 8.0f && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        hit.clickedNode = &node;
                        hit.clickedSocket = &socket;
                        hit.clickedOutput = true;
                    }

                    ++outputIndex;
                }
            }

            if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !hit.clickedSocket)
            {
                object.graph.selectedNodeId = node.id;

                state.dragging = true;
                state.dragNodeId = node.id;
                state.dragOffsetX = mousePos.x - pos.x;
                state.dragOffsetY = mousePos.y - pos.y;
                hit.nodeHit = true;
            }

            if (state.dragging && state.dragNodeId == node.id && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                node.x = mousePos.x - canvasOrigin.x - state.dragOffsetX;
                node.y = mousePos.y - canvasOrigin.y - state.dragOffsetY;
                hit.nodeHit = true;
            }

            if (hovered)
            {
                hit.nodeHit = true;
                hit.interactiveHovered = true;
            }
        }
    }
}

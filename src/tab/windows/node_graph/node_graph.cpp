#include "tab/windows/node_graph/node_graph.h"
#include "tab/windows/node_graph/node_graph_state.h"
#include "tab/windows/node_graph/node_graph_add_node.h"
#include "tab/windows/node_graph/node_graph_connections.h"
#include "tab/windows/node_graph/node_graph_nodes.h"
#include "tab/windows/node_graph/node_graph_input.h"

#include "scene/scene.h"
#include "ui/add_button.h"

#include "imgui.h"

#include <cmath>
#include <string>
#include <unordered_map>

namespace CNode
{
    namespace
    {
        std::unordered_map<std::string, NodeGraphState> nodeGraphs;

        NodeGraphState* GetNodeGraphState(const char* title)
        {
            if (!title || !*title)
                return nullptr;

            auto& state = nodeGraphs[title];
            return &state;
        }

        void DestroyNodeGraphState(const char* title)
        {
            if (!title || !*title)
                return;

            nodeGraphs.erase(title);
        }
    }

    void DrawNodeGraph(const char* title, bool* open)
    {
        if (!title || !*title)
            return;

        NodeGraphState* state = GetNodeGraphState(title);

        if (!state)
        {
            if (ImGui::Begin(title, open))
                ImGui::TextUnformatted("Failed to create node graph state.");

            ImGui::End();
            return;
        }

        if (!ImGui::Begin(title, open))
        {
            ImGui::End();

            if (open && !*open)
                DestroyNodeGraphState(title);

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

        UI::AddButton("AddNodePopup", "+ Add Node");

        DrawAddNodePopup(*object);

        ImGui::Separator();

        ImGui::BeginChild("NodeCanvas", ImVec2(0, 0), true);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 canvasOrigin = ImGui::GetCursorScreenPos();
        ImVec2 mousePos = ImGui::GetMousePos();

        NodeGraphColors colors;

        if (object->graph.nodes.empty())
        {
            ImGui::SetCursorPos(ImVec2(20, 30));
            ImGui::TextUnformatted("Empty graph. Use + to add a Cube.");
        }

        NodeGraphHitResult hit;

        DrawConnections(*object, drawList, colors);
        DrawConnectionPreview(*state, *object, drawList, mousePos, colors);

        DrawNodes(
            *state,
            *object,
            drawList,
            canvasOrigin,
            mousePos,
            colors,
            hit
        );

        HandleNodeGraphInput(*state, *object, canvasOrigin, mousePos, hit);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !hit.interactiveHovered)
            ImGui::OpenPopup("AddNodePopup");

        if (!state->dragging && !state->connecting && ImGui::IsWindowHovered() &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !hit.nodeHit && !hit.clickedSocket)
        {
            object->graph.selectedNodeId = 0;
        }

        ImGui::EndChild();

        ImGui::End();

        if (open && !*open)
            DestroyNodeGraphState(title);
    }
}

#include "tab/windows/inspector/inspector.h"
#include "tab/windows/inspector/inspector_add_component.h"
#include "tab/windows/inspector/inspector_components.h"

#include "scene/scene.h"
#include "ui/add_button.h"

#include "imgui.h"

namespace CNode
{
    void DrawInspector(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        SceneObject* object = GetSelectedSceneObject();

        if (!object)
        {
            ImGui::TextUnformatted("No object selected.");
            ImGui::End();
            return;
        }

        Scene& scene = GetScene();

        ImGui::Text("Object: %s", object->name.c_str());
        ImGui::Text("ID: %llu", static_cast<unsigned long long>(object->id));

        Node* selectedNode = nullptr;
        for (Node& node : object->graph.nodes)
        {
            if (node.id == object->graph.selectedNodeId)
            {
                selectedNode = &node;
                break;
            }
        }

        if (!selectedNode)
        {
            ImGui::Separator();
            ImGui::TextUnformatted("No node selected.");
            ImGui::End();
            return;
        }

        ImGui::Text("Node: %s (%s)", selectedNode->label.c_str(), selectedNode->type.c_str());

        UI::AddButton("InspectorAddComponent", "+ Add Component");

        DrawAddComponentPopup(*object);

        ImGui::Separator();

        ImGui::TextUnformatted("Components:");
        ImGui::SameLine();
        ImGui::TextDisabled("(drag header to reorder, right-click for menu)");

        DrawComponentList(*object);

        ImGui::End();
    }
}

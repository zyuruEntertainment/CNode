#include "tab/windows/inspector/inspector.h"
#include "scene/scene.h"

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

        ImGui::Text("Object: %s", object->name.c_str());
        ImGui::Text("ID: %llu", static_cast<unsigned long long>(object->id));

        if (object->selectedNodeId == 0)
        {
            ImGui::Separator();
            ImGui::TextUnformatted("No node selected.");
            ImGui::End();
            return;
        }

        Node* selectedNode = nullptr;

        for (Node& node : object->graph.nodes)
        {
            if (node.id == object->selectedNodeId)
            {
                selectedNode = &node;
                break;
            }
        }

        if (!selectedNode)
        {
            ImGui::Separator();
            ImGui::TextUnformatted("Selected node not found.");
            ImGui::End();
            return;
        }

        if (selectedNode->type == "cube")
        {
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat3(
                    "Position",
                    &object->transform.position.x,
                    0.01f
                );

                ImGui::DragFloat3(
                    "Rotation",
                    &object->transform.rotation.x,
                    0.1f
                );

                ImGui::DragFloat3(
                    "Scale",
                    &object->transform.scale.x,
                    0.01f
                );
            }

            if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Type: %s", object->mesh.type.c_str());
            }

            if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Checkbox("Enabled", &object->renderer.enabled);
                ImGui::ColorEdit3("Color", object->renderer.color);
            }
        }
        else
        {
            ImGui::Text("Unknown node type: %s", selectedNode->type.c_str());
        }

        ImGui::End();
    }
}
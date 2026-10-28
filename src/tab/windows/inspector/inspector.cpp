#include "tab/windows/inspector/inspector.h"
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

        // Add Component button (uses shared AddButton from src/ui/add_button).
        UI::AddButton("InspectorAddComponent", "+ Add Component");

        if (ImGui::BeginPopup("InspectorAddComponent"))
        {
            const char* availableTypes[] = { "Transform", "Mesh", "Renderer" };
            for (const char* type : availableTypes)
            {
                bool alreadyHas = false;
                for (const std::string& existing : scene.componentOrder)
                    if (existing == type)
                    {
                        alreadyHas = true;
                        break;
                    }

                if (!alreadyHas && ImGui::MenuItem(type))
                {
                    AddComponent(object->id, type);
                }
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        // Components list (order matches componentOrder; sockets follow the same order).
        ImGui::TextUnformatted("Components:");
        ImGui::SameLine();
        ImGui::TextDisabled("(drag header to reorder, right-click for menu)");

        for (int i = 0; i < static_cast<int>(scene.componentOrder.size()); ++i)
        {
            const std::string& componentType = scene.componentOrder[i];

            ImGui::PushID(i);

            // Component header row.
            ImGui::TextUnformatted(componentType.c_str());
            ImGui::SameLine();

            // Indicate whether this component has data.
            bool hasData = true;
            if (componentType == "Mesh")
                hasData = !object->mesh.type.empty();
            else if (componentType == "Renderer")
                hasData = object->renderer.enabled;

            if (!hasData)
                ImGui::TextDisabled(" (empty)");

            // Right-click context menu on the header.
            if (ImGui::BeginPopupContextItem("ComponentHeaderContextMenu"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    RemoveComponent(object->id, componentType);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Copy"))
                {
                    CopyComponent(object->id, componentType);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Paste"))
                {
                    PasteComponent(object->id, componentType);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            // Drag-drop source: initiates reorder drag from this header.
            if (ImGui::BeginDragDropSource())
            {
                int dragIndex = i;
                ImGui::SetDragDropPayload("COMPONENT_REORDER", &dragIndex, sizeof(int));
                ImGui::EndDragDropSource();
            }

            // Drag-drop target: accepts reorder drop on this header.
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_REORDER"))
                {
                    int fromIndex = *(const int*)payload->Data;
                    if (fromIndex != i)
                        MoveComponent(object->id, fromIndex, i);
                }
                ImGui::EndDragDropTarget();
            }

            // Component properties.
            if (ImGui::CollapsingHeader(componentType.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (componentType == "Transform")
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
                else if (componentType == "Mesh")
                {
                    ImGui::Text("Type: %s", object->mesh.type.empty() ? "-" : object->mesh.type.c_str());
                }
                else if (componentType == "Renderer")
                {
                    ImGui::Checkbox("Enabled", &object->renderer.enabled);
                    ImGui::ColorEdit3("Color", object->renderer.color);
                }
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        // Node properties (only when a node is selected).
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
            ImGui::TextUnformatted("No node selected.");
        }
        else if (selectedNode->type == "cube")
        {
            ImGui::Text("Node: %s", selectedNode->label.c_str());

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

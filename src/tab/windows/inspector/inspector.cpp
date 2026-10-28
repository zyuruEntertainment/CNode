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

        // Components list (order matches componentOrder; sockets follow same order).
        ImGui::TextUnformatted("Components:");
        ImGui::SameLine();
        ImGui::TextDisabled("(drag header to reorder, right-click for menu)");

        // Deferred reorder: do NOT modify componentOrder during ImGui iteration
        // (it invalidates indices and causes abort()). Collect and apply after.
        int reorderFrom = -1;
        int reorderTo = -1;

        for (int i = 0; i < static_cast<int>(scene.componentOrder.size()); ++i)
        {
            const std::string& componentType = scene.componentOrder[i];

            ImGui::PushID(i);

            // Header row: component name + drag-drop + context menu.
            ImGui::TextUnformatted(componentType.c_str());

            // Drag-drop source.
            if (ImGui::BeginDragDropSource())
            {
                int dragIndex = i;
                ImGui::SetDragDropPayload("COMPONENT_REORDER", &dragIndex, sizeof(int));
                ImGui::EndDragDropSource();
            }

            // Drag-drop target.
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_REORDER"))
                {
                    int fromIndex = *(const int*)payload->Data;
                    reorderFrom = fromIndex;
                    reorderTo = i;
                }
                ImGui::EndDragDropTarget();
            }

            ImGui::SameLine();

            // Right-click context menu on header.
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

            // Show whether component has data.
            bool hasData = true;
            if (componentType == "Mesh")
                hasData = !object->mesh.type.empty();
            else if (componentType == "Renderer")
                hasData = object->renderer.enabled;

            if (!hasData)
            {
                ImGui::SameLine();
                ImGui::TextDisabled(" (empty)");
            }

            ImGui::PopID();

            // Component properties (collapsing header).
            if (ImGui::CollapsingHeader(componentType.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (componentType == "Transform")
                {
                    ImGui::DragFloat3("Position", &object->transform.position.x, 0.01f);
                    ImGui::DragFloat3("Rotation", &object->transform.rotation.x, 0.1f);
                    ImGui::DragFloat3("Scale", &object->transform.scale.x, 0.01f);
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
        }

        // Apply deferred reorder after ImGui iteration is done.
        if (reorderFrom != -1 && reorderTo != -1 && reorderFrom != reorderTo)
        {
            MoveComponent(object->id, reorderFrom, reorderTo);
        }

        ImGui::Separator();

        // Node info (only when a node is selected).
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
        else
        {
            ImGui::Text("Node: %s (%s)", selectedNode->label.c_str(), selectedNode->type.c_str());
        }

        ImGui::End();
    }
}

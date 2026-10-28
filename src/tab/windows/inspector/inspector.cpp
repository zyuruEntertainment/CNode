#include "tab/windows/inspector/inspector.h"
#include "scene/scene.h"
#include "ui/add_button.h"

#include "imgui.h"

#include <filesystem>
#include <cctype>
#include <algorithm>

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

        // Find the currently selected node (if any).
        Node* selectedNode = nullptr;
        for (Node& node : object->graph.nodes)
        {
            if (node.id == object->graph.selectedNodeId)
            {
                selectedNode = &node;
                break;
            }
        }

        // If no node is selected, show a message and do not render components.
        if (!selectedNode)
        {
            ImGui::Separator();
            ImGui::TextUnformatted("No node selected.");
            ImGui::End();
            return;
        }

        ImGui::Text("Node: %s (%s)", selectedNode->label.c_str(), selectedNode->type.c_str());

        // Add Component button (uses shared AddButton from src/ui/add_button).
        UI::AddButton("InspectorAddComponent", "+ Add Component");

        if (ImGui::BeginPopup("InspectorAddComponent"))
        {
            // Scan components directory for available component types.
            std::vector<std::string> availableTypes;
            namespace fs = std::filesystem;
            fs::path componentsDir = "include/components";
            if (fs::exists(componentsDir) && fs::is_directory(componentsDir))
            {
                for (const auto& entry : fs::directory_iterator(componentsDir))
                {
                    if (entry.is_directory())
                    {
                        std::string name = entry.path().filename().string();
                        if (!name.empty())
                            name[0] = static_cast<char>(
                                std::toupper(static_cast<unsigned char>(name[0])));
                        availableTypes.push_back(name);
                    }
                }
            }
            std::sort(availableTypes.begin(), availableTypes.end());

            for (const auto& type : availableTypes)
            {
                bool alreadyHas = HasComponent(object->id, type);
                if (!alreadyHas && ImGui::MenuItem(type.c_str()))
                {
                    AddComponent(object->id, type);
                }
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        // Components list (order matches object->componentOrder; sockets follow same order).
        ImGui::TextUnformatted("Components:");
        ImGui::SameLine();
        ImGui::TextDisabled("(drag header to reorder, right-click for menu)");

        // Deferred reorder: capture target indices during drag, apply only on
        // mouse release to avoid mutating the vector while ImGui drag-drop is
        // active (which causes abort()).
        int reorderFrom = -1;
        int reorderTo = -1;

        // Deferred removal: collect component types to delete, apply after the
        // loop to avoid mutating the vector while ImGui is rendering it.
        std::vector<std::string> deferredRemovals;

        for (int i = 0; i < static_cast<int>(object->componentOrder.size()); ++i)
        {
            // Copy by value so the reference stays valid even if
            // RemoveComponent mutates componentOrder inside the context menu.
            std::string componentType = object->componentOrder[i];

            ImGui::PushID(i);

            // Component header (collapsing). This is the interactive header item:
            // clicking toggles open/close, left-click-hold drags to reorder,
            // right-click opens the context menu.
            bool headerOpen = ImGui::CollapsingHeader(
                componentType.c_str(),
                ImGuiTreeNodeFlags_DefaultOpen
            );

            // Drag-drop source on header (left-click hold).
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                int dragIndex = i;
                ImGui::SetDragDropPayload("COMPONENT_REORDER", &dragIndex, sizeof(int));
                ImGui::EndDragDropSource();
            }

            // Drag-drop target on header: capture hover target, apply on release.
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

            // Right-click context menu on header.
            if (ImGui::BeginPopupContextItem("ComponentHeaderContextMenu"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    deferredRemovals.push_back(componentType);
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

            if (headerOpen)
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

            ImGui::PopID();
        }

        // Apply deferred reorder only when the drag is completed (mouse released).
        if (reorderFrom != -1 && reorderTo != -1 && reorderFrom != reorderTo &&
            ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            MoveComponent(object->id, reorderFrom, reorderTo);
        }

        // Apply deferred component removals now that ImGui rendering is done.
        for (const std::string& type : deferredRemovals)
            RemoveComponent(object->id, type);

        ImGui::End();
    }
}

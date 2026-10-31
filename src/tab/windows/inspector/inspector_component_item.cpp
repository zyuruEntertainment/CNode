#include "tab/windows/inspector/inspector_component_item.h"

#include "imgui.h"

namespace CNode
{
    void DrawComponentItem(
        SceneObject& object,
        const std::string& componentType,
        int index,
        int& reorderFrom,
        int& reorderTo,
        std::vector<std::string>& deferredRemovals
    )
    {
        ImGui::PushID(index);

        bool headerOpen = ImGui::CollapsingHeader(
            componentType.c_str(),
            ImGuiTreeNodeFlags_DefaultOpen
        );

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            int dragIndex = index;
            ImGui::SetDragDropPayload("COMPONENT_REORDER", &dragIndex, sizeof(int));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("COMPONENT_REORDER"))
            {
                int fromIndex = *(const int*)payload->Data;
                reorderFrom = fromIndex;
                reorderTo = index;
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem("ComponentHeaderContextMenu"))
        {
            if (ImGui::MenuItem("Delete"))
            {
                deferredRemovals.push_back(componentType);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Copy"))
            {
                CopyComponent(object.id, componentType);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Paste"))
            {
                PasteComponent(object.id, componentType);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        bool hasData = true;
        if (componentType == "Mesh")
            hasData = !object.mesh.type.empty();
        else if (componentType == "Renderer")
            hasData = object.renderer.enabled;

        if (!hasData)
        {
            ImGui::SameLine();
            ImGui::TextDisabled(" (empty)");
        }

        if (headerOpen)
        {
            if (componentType == "Transform")
            {
                ImGui::DragFloat3("Position", &object.transform.position.x, 0.01f);
                ImGui::DragFloat3("Rotation", &object.transform.rotation.x, 0.1f);
                ImGui::DragFloat3("Scale", &object.transform.scale.x, 0.01f);
            }
            else if (componentType == "Mesh")
            {
                ImGui::Text("Type: %s", object.mesh.type.empty() ? "-" : object.mesh.type.c_str());
            }
            else if (componentType == "Renderer")
            {
                ImGui::Checkbox("Enabled", &object.renderer.enabled);
                ImGui::ColorEdit3("Color", object.renderer.color);
            }
        }

        ImGui::PopID();
    }
}

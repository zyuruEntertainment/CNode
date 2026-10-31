#include "tab/windows/object_list/object_list_item.h"
#include "tab/windows/object_list/object_list_rename.h"

#include "imgui.h"

#include <string>

namespace CNode
{
    void DrawObjectListItem(
        ObjectListState& state,
        Scene& scene,
        SceneObject& object
    )
    {
        const bool selected = (scene.selectedObjectId == object.id);

        if (state.renaming && state.renamingId == object.id)
        {
            if (ImGui::InputText(
                    "##rename",
                    state.renameBuffer,
                    sizeof(state.renameBuffer),
                    ImGuiInputTextFlags_EnterReturnsTrue |
                    ImGuiInputTextFlags_AutoSelectAll))
            {
                std::string newName = state.renameBuffer;

                bool onlyWhitespace = true;

                for (char c : newName)
                {
                    if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
                    {
                        onlyWhitespace = false;
                        break;
                    }
                }

                if (!onlyWhitespace)
                    RenameSceneObject(object.id, newName);

                state.renaming = false;
                state.renamingId = 0;
            }

            if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            {
                state.renaming = false;
                state.renamingId = 0;
            }
        }
        else
        {
            if (ImGui::Selectable(object.name.c_str(), selected))
            {
                scene.selectedObjectId = object.id;
            }

            if (ImGui::IsItemHovered())
                state.itemHovered = true;

            if (selected && ImGui::IsItemHovered() &&
                ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                ImGui::OpenPopup("ObjectContextMenu");
            }

            if (selected && ImGui::BeginPopup("ObjectContextMenu"))
            {
                if (ImGui::MenuItem("Delete"))
                {
                    DeleteSceneObject(object.id);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Copy"))
                {
                    CopySceneObject(object.id);
                    ImGui::CloseCurrentPopup();
                }
                if (ImGui::MenuItem("Paste"))
                {
                    PasteSceneObject();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (selected && ImGui::IsKeyPressed(ImGuiKey_F2))
                StartRename(state, object);

            if (ImGui::IsItemHovered() &&
                ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                StartRename(state, object);
        }
    }
}

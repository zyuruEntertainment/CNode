#include "tab/windows/object_list/object_list.h"
#include "tab/windows/object_list/object_list_state.h"
#include "tab/windows/object_list/object_list_item.h"
#include "tab/windows/object_list/object_list_rename.h"
#include "tab/windows/object_list/object_list_context_menu.h"
#include "tab/windows/object_list/object_list_keyboard.h"

#include "scene/scene.h"
#include "ui/add_button.h"

#include "imgui.h"

#include <string>
#include <unordered_map>

namespace CNode
{
    namespace
    {
        std::unordered_map<std::string, ObjectListState> objectLists;

        ObjectListState* GetObjectListState(const char* title)
        {
            if (!title || !*title)
                return nullptr;

            auto& state = objectLists[title];
            return &state;
        }

        void DestroyObjectListState(const char* title)
        {
            if (!title || !*title)
                return;

            objectLists.erase(title);
        }
    }

    void DrawObjectList(const char* title, bool* open)
    {
        if (!title || !*title)
            return;

        ObjectListState* state = GetObjectListState(title);

        if (!state)
        {
            if (ImGui::Begin(title, open))
                ImGui::TextUnformatted("Failed to create object list state.");

            ImGui::End();
            return;
        }

        if (!ImGui::Begin(title, open))
        {
            ImGui::End();

            if (open && !*open)
                DestroyObjectListState(title);

            return;
        }

        Scene& scene = GetScene();

        state->itemHovered = false;

        UI::AddButton("ObjectListContextMenu", "+ Add Object");

        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
            !state->itemHovered)
        {
            ImGui::OpenPopup("ObjectListContextMenu");
        }

        DrawAddObjectMenu(*state, scene, title);

        ImGui::Separator();

        for (SceneObject& object : scene.objects)
        {
            DrawObjectListItem(*state, scene, object);
        }

        HandleObjectKeyboardShortcuts(scene);

        ImGui::End();

        if (open && !*open)
            DestroyObjectListState(title);
    }
}

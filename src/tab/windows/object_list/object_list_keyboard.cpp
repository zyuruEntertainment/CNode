#include "tab/windows/object_list/object_list_keyboard.h"

#include "imgui.h"

namespace CNode
{
    void HandleObjectKeyboardShortcuts(Scene& scene)
    {
        if (scene.selectedObjectId != 0)
        {
            if (ImGui::IsKeyPressed(ImGuiKey_Delete))
            {
                DeleteSceneObject(scene.selectedObjectId);
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_C) &&
                     ImGui::GetIO().KeyCtrl)
            {
                CopySceneObject(scene.selectedObjectId);
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_V) && ImGui::GetIO().KeyCtrl)
        {
            PasteSceneObject();
        }
    }
}

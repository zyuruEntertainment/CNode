#include "tab/windows/object_list/object_list_context_menu.h"
#include "tab/windows/object_list/object_list_rename.h"

#include "imgui.h"

#include <string>

namespace CNode
{
    namespace
    {
        std::string MakeUniqueObjectName(Scene& scene, const char* desired)
        {
            auto nameExists = [&](const std::string& name)
            {
                for (const SceneObject& object : scene.objects)
                    if (object.name == name)
                        return true;
                return false;
            };

            std::string name = desired;
            if (!nameExists(name))
                return name;

            for (int suffix = 2; ; ++suffix)
            {
                std::string candidate = name + " " + std::to_string(suffix);
                if (!nameExists(candidate))
                    return candidate;
            }
        }

        void CreateNewObject(ObjectListState& state, Scene& scene)
        {
            SceneObject& object = CreateSceneObject(MakeUniqueObjectName(scene, "NewObject"));
            scene.selectedObjectId = object.id;
            StartRename(state, object);
        }
    }

    void DrawAddObjectMenu(ObjectListState& state, Scene& scene, const char* title)
    {
        if (ImGui::BeginPopup("ObjectListContextMenu"))
        {
            if (ImGui::MenuItem("Add Object"))
            {
                CreateNewObject(state, scene);
                ImGui::SetWindowFocus(title);
            }
            ImGui::EndPopup();
        }
    }
}

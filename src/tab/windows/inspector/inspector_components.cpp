#include "tab/windows/inspector/inspector_components.h"
#include "tab/windows/inspector/inspector_component_item.h"

#include "imgui.h"

#include <vector>
#include <string>

namespace CNode
{
    void DrawComponentList(SceneObject& object)
    {
        int reorderFrom = -1;
        int reorderTo = -1;
        std::vector<std::string> deferredRemovals;

        for (int i = 0; i < static_cast<int>(object.componentOrder.size()); ++i)
        {
            std::string componentType = object.componentOrder[i];

            ImGui::PushID(i);

            DrawComponentItem(
                object,
                componentType,
                i,
                reorderFrom,
                reorderTo,
                deferredRemovals
            );

            ImGui::PopID();
        }

        if (reorderFrom != -1 && reorderTo != -1 && reorderFrom != reorderTo &&
            ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            MoveComponent(object.id, reorderFrom, reorderTo);
        }

        for (const std::string& type : deferredRemovals)
            RemoveComponent(object.id, type);
    }
}

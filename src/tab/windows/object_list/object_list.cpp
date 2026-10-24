#include "tab/windows/object_list/object_list.h"

#include "imgui.h"

namespace CNode
{
    void DrawObjectList(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        ImGui::End();
    }
}
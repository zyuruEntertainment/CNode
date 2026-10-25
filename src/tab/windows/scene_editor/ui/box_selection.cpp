#include "tab/windows/scene_editor/ui/box_selection.h"

#include "imgui.h"

#include <algorithm>

namespace CNode
{
    bool BoxSelection::IsSelecting() const
    {
        return selecting;
    }

    void BoxSelection::Draw(
        GLFWwindow* window,
        const ImVec2& viewportMin,
        const ImVec2& viewportMax,
        bool viewportHovered,
        bool gizmoBusy,
        bool orbitGizmoActive,
        bool panControllerActive,
        bool zoomControllerActive,
        bool freeLookActive,
        bool panActive,
        bool orbitActive
    )
    {
        (void)window;
        (void)orbitGizmoActive;
        (void)panControllerActive;
        (void)zoomControllerActive;
        (void)freeLookActive;
        (void)panActive;
        (void)orbitActive;

        const ImGuiIO& io = ImGui::GetIO();

        if (!selecting)
        {
            if (gizmoBusy)
                return;

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
                viewportHovered &&
                !io.KeyShift &&
                !io.KeyAlt &&
                !io.KeyCtrl)
            {
                selecting = true;
                startedInViewport = true;
                startPos = io.MousePos;
                endPos = io.MousePos;
            }

            return;
        }

        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            selecting = false;
            startedInViewport = false;
            return;
        }

        endPos = io.MousePos;

        const ImVec2 minPos(
            std::min(startPos.x, endPos.x),
            std::min(startPos.y, endPos.y)
        );

        const ImVec2 maxPos(
            std::max(startPos.x, endPos.x),
            std::max(startPos.y, endPos.y)
        );

        if (maxPos.x - minPos.x < 1.0f &&
            maxPos.y - minPos.y < 1.0f)
        {
            return;
        }

        ImDrawList* drawList = ImGui::GetForegroundDrawList();

        drawList->AddRectFilled(
            minPos,
            maxPos,
            IM_COL32(90, 150, 255, 40)
        );

        drawList->AddRect(
            minPos,
            maxPos,
            IM_COL32(120, 170, 255, 200),
            0.0f,
            0,
            1.0f
        );
    }
}

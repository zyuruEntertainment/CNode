#include "ui/add_button.h"

#include "imgui.h"

namespace CNode::UI
{
    namespace
    {
        void PushAddButtonStyle()
        {
            ImGui::PushStyleVar(
                ImGuiStyleVar_FrameRounding,
                6.0f
            );

            ImGui::PushStyleVar(
                ImGuiStyleVar_FramePadding,
                ImVec2(8.0f, 4.0f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_Button,
                ImVec4(0.26f, 0.26f, 0.30f, 1.0f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered,
                ImVec4(0.36f, 0.36f, 0.40f, 1.0f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_ButtonActive,
                ImVec4(0.46f, 0.46f, 0.50f, 1.0f)
            );
        }

        void PopAddButtonStyle()
        {
            ImGui::PopStyleColor(3);
            ImGui::PopStyleVar(2);
        }
    }

    bool AddButton(const char* popupID)
    {
        PushAddButtonStyle();

        const bool clicked = ImGui::Button("+");

        PopAddButtonStyle();

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Add");

        if (clicked && popupID != nullptr && *popupID != '\0')
            ImGui::OpenPopup(popupID);

        return clicked;
    }
}

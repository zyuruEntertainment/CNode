#include "ui/add_button.h"

#include "imgui.h"

namespace CNode::UI
{
    namespace
    {
        // Fallback sequence used to namespace AddButton instances that are not
        // tied to a popupID. Guarantees a unique ImGui ID per button even when
        // several buttons share the same label inside a single window.
        int gAddButtonSequence = 0;

        void PushAddButtonStyle()
        {
            ImGui::PushStyleVar(
                ImGuiStyleVar_FrameRounding,
                6.0f
            );

            ImGui::PushStyleVar(
                ImGuiStyleVar_FramePadding,
                ImVec2(12.0f, 6.0f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_Text,
                ImVec4(1.00f, 1.00f, 1.00f, 1.0f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_Button,
                ImVec4(0.20f, 0.42f, 0.72f, 1.00f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_ButtonHovered,
                ImVec4(0.27f, 0.52f, 0.84f, 1.00f)
            );

            ImGui::PushStyleColor(
                ImGuiCol_ButtonActive,
                ImVec4(0.16f, 0.36f, 0.64f, 1.00f)
            );
        }

        void PopAddButtonStyle()
        {
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar(2);
        }
    }

    bool AddButton(const char* popupID, const char* label)
    {
        if (label == nullptr || *label == '\0')
            label = "+  Add";

        PushAddButtonStyle();

        // ImGui derives a widget's ID from its label text, so two buttons with
        // the same label in one window collide ("conflicting ID"). Scope the
        // button under popupID (unique per window: "ObjectListContextMenu",
        // "AddNodePopup", ...) so identical labels never conflict. When no
        // popupID is given, fall back to a per-process counter.
        if (popupID != nullptr && *popupID != '\0')
            ImGui::PushID(popupID);
        else
            ImGui::PushID(gAddButtonSequence++);

        const bool clicked = ImGui::Button(label);

        ImGui::PopID();

        PopAddButtonStyle();

        if (clicked && popupID != nullptr && *popupID != '\0')
            ImGui::OpenPopup(popupID);

        return clicked;
    }
}

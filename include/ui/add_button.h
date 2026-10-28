#pragma once

namespace CNode::UI
{
    // Compact "+" button shared by every window that creates something
    // (Object List, Node Graph, ...). Keeps the plus button style
    // consistent across the editor.
    //
    // Draws the button and, when clicked, opens popupID so the caller can
    // draw its own modular menu:
    //
    //   UI::AddButton("ObjectListContextMenu");
    //
    //   if (ImGui::BeginPopup("ObjectListContextMenu"))
    //   {
    //       if (ImGui::MenuItem("Add Object"))
    //           CreateNewObject();
    //
    //       ImGui::EndPopup();
    //   }
    //
    // Pass nullptr to only draw the button and handle the click yourself.
    // Returns true when the button was pressed this frame.
    bool AddButton(const char* popupID = nullptr);
}

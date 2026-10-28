#pragma once

namespace CNode::UI
{
    // Shared "add" button used by every window that creates something
    // (Object List, Node Graph, ...). Keeps the button readable and
    // consistent across the editor, and gives each window a modular popup
    // to fill in with its own menu entries:
    //
    //   UI::AddButton("ObjectListContextMenu", "+ Add Object");
    //
    //   if (ImGui::BeginPopup("ObjectListContextMenu"))
    //   {
    //       if (ImGui::MenuItem("Add Object"))
    //           CreateNewObject();
    //
    //       ImGui::EndPopup();
    //   }
    //
    // Draws the button and, when clicked, opens popupID.
    // Pass nullptr for popupID to handle the click yourself.
    // Returns true when the button was pressed this frame.
    bool AddButton(
        const char* popupID = nullptr,
        const char* label = "+  Add"
    );
}

#include "tab/windows/node_graph/node_graph_add_node.h"

#include "imgui.h"

namespace CNode
{
    void DrawAddNodePopup(SceneObject& object)
    {
        if (ImGui::BeginPopup("AddNodePopup"))
        {
            if (ImGui::MenuItem("Cube"))
            {
                CreateCubeNode(object);
            }

            ImGui::EndPopup();
        }
    }
}

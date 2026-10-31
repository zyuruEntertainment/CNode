#pragma once

#include "tab/windows/node_graph/node_graph_state.h"

#include "scene/scene.h"

#include "imgui.h"

namespace CNode
{
    void DrawNodes(
        NodeGraphState& state,
        SceneObject& object,
        ImDrawList* drawList,
        ImVec2 canvasOrigin,
        ImVec2 mousePos,
        const NodeGraphColors& colors,
        NodeGraphHitResult& hit
    );
}

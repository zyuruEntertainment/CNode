#pragma once

#include "tab/windows/node_graph/node_graph_state.h"

#include "scene/scene.h"

#include "imgui.h"

namespace CNode
{
    void HandleNodeGraphInput(
        NodeGraphState& state,
        SceneObject& object,
        ImVec2 canvasOrigin,
        ImVec2 mousePos,
        const NodeGraphHitResult& hit
    );
}

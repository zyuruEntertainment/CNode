#pragma once

#include "tab/windows/window_registry.h"
#include "tab/windows/node_graph/node_graph.h"

namespace CNode
{
    inline void RegisterFeature_tab_windows_node_graph_node_graph()
    {
        RegisterWindowAttribute(
            "node_graph",
            "Node Graph",
            &DrawNodeGraph,
            ImGuiDir_None
        );
    }
}
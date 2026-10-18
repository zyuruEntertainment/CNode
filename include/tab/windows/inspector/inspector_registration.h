#pragma once

#include "tab/windows/window_registry.h"
#include "tab/windows/inspector/inspector.h"

namespace CNode
{
    inline void RegisterFeature_tab_windows_inspector_inspector()
    {
        RegisterWindowAttribute(
            "inspector",
            "Inspector",
            &DrawInspector,
            ImGuiDir_None
        );
    }
}
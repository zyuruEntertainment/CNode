#pragma once

#include "tab/windows/window_registry.h"
#include "tab/windows/scene_editor/scene_editor.h"

namespace CNode
{
    inline void RegisterFeature_tab_windows_scene_editor_scene_editor()
    {
        RegisterWindowAttribute(
            "scene_editor",
            "Scene Editor",
            &DrawSceneEditor,
            ImGuiDir_None
        );
    }
}
#pragma once

#include "tab/windows/window_registry.h"
#include "tab/windows/project_setting/project_setting.h"

namespace CNode
{
    inline void RegisterFeature_tab_windows_project_setting_project_setting()
    {
        RegisterWindowAttribute(
            "project_setting",
            "Project Setting",
            &DrawProjectSetting,
            ImGuiDir_None
        );
    }
}
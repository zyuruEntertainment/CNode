#include "tab/windows/project_setting/axis_settings.h"

#include "imgui.h"

namespace CNode
{
    void DrawAxisSettings(ProjectSettings& settings)
    {
        ImGui::Checkbox("Enable Axis", &settings.showAxis);
    }
}
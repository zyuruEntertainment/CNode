#include "tab/windows/project_setting/viewport_settings.h"
#include "tab/windows/project_setting/grid_settings.h"
#include "tab/windows/project_setting/axis_settings.h"

#include "imgui.h"

namespace CNode
{
    void DrawViewportSettings(ProjectSettings& settings)
    {
        ImGui::TextUnformatted("Viewport Overlays");
        DrawGridSettings(settings);
        DrawAxisSettings(settings);
    }
}
#include "tab/windows/project_setting/grid_settings.h"

#include "imgui.h"

namespace CNode
{
    void DrawGridSettings(ProjectSettings& settings)
    {
        ImGui::Checkbox("Enable Grid", &settings.showGrid);

        if (!settings.showGrid)
            return;

        ImGui::SliderFloat(
            "Grid Render Distance",
            &settings.gridRenderDistance,
            50.0f,
            2000.0f,
            "%.0f"
        );

        ImGui::SliderFloat(
            "Grid Opacity",
            &settings.gridOpacity,
            0.0f,
            1.0f,
            "%.2f"
        );
    }
}
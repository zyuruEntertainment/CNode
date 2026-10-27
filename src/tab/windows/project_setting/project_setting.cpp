#include "tab/windows/project_setting/project_setting.h"

#include "imgui.h"

namespace CNode
{
    namespace
    {
        ProjectSettings settings;
    }

    ProjectSettings& GetProjectSettings()
    {
        return settings;
    }

    void DrawProjectSetting(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted("Environment");
        ImGui::ColorEdit3(
            "Background Color",
            settings.backgroundColor
        );

        ImGui::Separator();

        ImGui::TextUnformatted("Viewport Overlays");
        ImGui::Checkbox("Enable Grid", &settings.showGrid);
        ImGui::Checkbox("Enable Axis", &settings.showAxis);

        if (settings.showGrid)
        {
            ImGui::SliderFloat(
                "Grid Render Distance",
                &settings.gridRenderDistance,
                50.0f,
                2000.0f,
                "%.0f"
            );
        }

        ImGui::End();
    }
}
#include "tab/windows/project_setting/environment_settings.h"

#include "imgui.h"

namespace CNode
{
    void DrawEnvironmentSettings(ProjectSettings& settings)
    {
        ImGui::TextUnformatted("Environment");
        ImGui::ColorEdit3(
            "Background Color",
            settings.backgroundColor
        );
    }
}
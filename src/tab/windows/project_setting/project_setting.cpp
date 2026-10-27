#include "tab/windows/project_setting/project_setting.h"
#include "tab/windows/project_setting/environment_settings.h"
#include "tab/windows/project_setting/viewport_settings.h"

#include "imgui.h"

namespace CNode
{
    void DrawProjectSetting(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        ProjectSettings& settings = GetProjectSettings();

        DrawEnvironmentSettings(settings);

        ImGui::Separator();

        DrawViewportSettings(settings);

        ImGui::End();
    }
}
#pragma once

#include <array>

namespace CNode
{
    struct ProjectSettings
    {
        bool showGrid = true;
        bool showAxis = true;
        float gridRenderDistance = 400.0f;
        float gridOpacity = 0.75f;

        float backgroundColor[3] = { 0.075f, 0.085f, 0.10f };
    };

    ProjectSettings& GetProjectSettings();
    void DrawProjectSetting(const char* title, bool* open);
}
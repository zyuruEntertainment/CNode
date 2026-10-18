
#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace CNode
{
    using WindowDrawFunction = void (*)(const char* title, bool* open);

    struct WindowType
    {
        std::string id;
        std::string name;
        WindowDrawFunction draw = nullptr;
        ImGuiDir defaultDockDirection = ImGuiDir_None;
    };

    bool RegisterWindowType(
        const char* id,
        const char* name,
        WindowDrawFunction draw,
        ImGuiDir defaultDockDirection = ImGuiDir_None
    );

    bool RegisterWindowAttribute(
        const char* id,
        const char* name,
        WindowDrawFunction draw,
        ImGuiDir defaultDockDirection = ImGuiDir_None
    );

    void RegisterWindowAttributes();

    const std::vector<WindowType>& GetRegisteredWindows();
}

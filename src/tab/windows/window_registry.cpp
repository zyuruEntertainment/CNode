
#include "tab/windows/window_registry.h"
#include "tab/windows/window_id.h"

namespace CNode
{
    namespace
    {
        std::vector<WindowType>& GetWindowTypeStorage()
        {
            static std::vector<WindowType> windows;
            return windows;
        }

        std::vector<WindowType>& GetWindowAttributeStorage()
        {
            static std::vector<WindowType> attributes;
            return attributes;
        }
    }

    bool RegisterWindowType(
        const char* id,
        const char* name,
        WindowDrawFunction draw,
        ImGuiDir defaultDockDirection)
    {
        if (!IsValidWindowID(id) ||
            name == nullptr ||
            *name == '\0' ||
            draw == nullptr)
        {
            return false;
        }

        auto& windows = GetWindowTypeStorage();

        if (!RegisterWindowTypeID(id))
            return false;

        windows.push_back({
            id,
            name,
            draw,
            defaultDockDirection
            });

        return true;
    }

    bool RegisterWindowAttribute(
        const char* id,
        const char* name,
        WindowDrawFunction draw,
        ImGuiDir defaultDockDirection)
    {
        if (!IsValidWindowID(id) ||
            name == nullptr ||
            *name == '\0' ||
            draw == nullptr)
        {
            return false;
        }

        auto& attributes = GetWindowAttributeStorage();

        if (!RegisterWindowAttributeID(id))
            return false;

        attributes.push_back({
            id,
            name,
            draw,
            defaultDockDirection
            });

        return true;
    }

    void RegisterWindowAttributes()
    {
        const auto& attributes = GetWindowAttributeStorage();

        for (const WindowType& attribute : attributes)
        {
            RegisterWindowType(
                attribute.id.c_str(),
                attribute.name.c_str(),
                attribute.draw,
                attribute.defaultDockDirection
            );
        }
    }

    const std::vector<WindowType>& GetRegisteredWindows()
    {
        return GetWindowTypeStorage();
    }
}

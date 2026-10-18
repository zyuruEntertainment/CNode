#pragma once

#include <list>
#include <string>

namespace CNode
{
    struct WindowInstance
    {
        unsigned long long instanceID = 0;
        std::string typeID;
        std::string title;
        bool open = true;
    };

    WindowInstance* AddWindow(const char* typeID);

    std::list<WindowInstance>& GetWindowInstances();

    void RemoveClosedWindowInstances();
}

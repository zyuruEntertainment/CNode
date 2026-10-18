#pragma once

#include "registration/registration_data.h"
#include "tab/windows/object_list/object_list.h"

namespace CNode
{
    inline void RegisterFeature_tab_windows_object_list_object_list()
    {
        RegisterWindowAttribute(
            "object_list",
            "Object List",
            &DrawObjectList,
            ImGuiDir_None
        );
    }
}
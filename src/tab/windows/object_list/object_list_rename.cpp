#include "tab/windows/object_list/object_list_rename.h"

#include <cstdio>
#include <string>

namespace CNode
{
    void StartRename(ObjectListState& state, SceneObject& object)
    {
        state.renaming = true;
        state.renamingId = object.id;
        std::string name = object.name;
        std::snprintf(state.renameBuffer, sizeof(state.renameBuffer), "%s", name.c_str());
    }
}

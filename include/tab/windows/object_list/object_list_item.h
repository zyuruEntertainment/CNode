#pragma once

#include "tab/windows/object_list/object_list_state.h"
#include "scene/scene.h"

namespace CNode
{
    void DrawObjectListItem(
        ObjectListState& state,
        Scene& scene,
        SceneObject& object
    );
}

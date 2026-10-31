#pragma once

#include "scene/scene.h"

#include <string>
#include <vector>

namespace CNode
{
    void DrawComponentItem(
        SceneObject& object,
        const std::string& componentType,
        int index,
        int& reorderFrom,
        int& reorderTo,
        std::vector<std::string>& deferredRemovals
    );
}

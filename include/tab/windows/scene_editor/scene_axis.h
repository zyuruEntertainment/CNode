#pragma once

#include "tab/windows/scene_editor/scene_camera.h"
#include "tab/windows/scene_editor/scene_grid.h"

#include <vector>

namespace CNode
{
    std::vector<SceneLineVertex> BuildSceneAxisVertices(const SceneCamera& camera);
}
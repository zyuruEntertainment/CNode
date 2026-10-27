#pragma once

#include "components/transform/transform_component.h"
#include "components/mesh/mesh_component.h"
#include "components/renderer/renderer_component.h"

#include "nodes/node_graph.h"

#include <string>
#include <vector>

namespace CNode
{
    struct SceneObject
    {
        unsigned long long id = 0;
        std::string name = "NewObject";

        TransformComponent transform{};
        MeshComponent mesh{};
        RendererComponent renderer{};

        NodeGraph graph{};
        unsigned long long selectedNodeId = 0;
    };

    struct Scene
    {
        std::vector<SceneObject> objects;
        unsigned long long selectedObjectId = 0;
        unsigned long long nextObjectId = 1;
        unsigned long long nextNodeId = 1;
    };

    Scene& GetScene();

    SceneObject* GetSelectedSceneObject();
    SceneObject* FindSceneObject(unsigned long long id);

    SceneObject& CreateSceneObject(const std::string& name);
    void CreateCubeNode(SceneObject& object);

    bool RenameSceneObject(unsigned long long id, const std::string& name);
}
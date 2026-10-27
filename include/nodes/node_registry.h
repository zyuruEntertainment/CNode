#pragma once

#include "nodes/node.h"

#include <functional>
#include <string>
#include <vector>

namespace CNode
{
    struct SceneObject;

    using NodeFactory = std::function<Node(SceneObject&, float, float)>;

    struct RegisteredNodeType
    {
        std::string type;
        std::string label;
        NodeFactory factory;
    };

    void RegisterNodeType(
        const std::string& type,
        const std::string& label,
        const NodeFactory& factory
    );

    const std::vector<RegisteredNodeType>& GetRegisteredNodeTypes();

    void RegisterBuiltinNodes();
}
#include "nodes/node_registry.h"
#include "nodes/templates/cube_node.h"

#include <algorithm>

namespace CNode
{
    namespace
    {
        std::vector<RegisteredNodeType>& GetNodeTypeStorage()
        {
            static std::vector<RegisteredNodeType> types;
            return types;
        }
    }

    void RegisterNodeType(
        const std::string& type,
        const std::string& label,
        const NodeFactory& factory)
    {
        auto& types = GetNodeTypeStorage();

        for (const RegisteredNodeType& existing : types)
        {
            if (existing.type == type)
                return;
        }

        types.push_back({ type, label, factory });
    }

    const std::vector<RegisteredNodeType>& GetRegisteredNodeTypes()
    {
        return GetNodeTypeStorage();
    }

    void RegisterBuiltinNodes()
    {
        static bool registered = false;

        if (registered)
            return;

        registered = true;
        RegisterCubeNodeType();
    }
}
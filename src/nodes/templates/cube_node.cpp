#include "nodes/templates/cube_node.h"
#include "nodes/node_registry.h"
#include "scene/scene.h"

namespace CNode
{
    namespace
    {
        Node CreateCubeNodeInstance(SceneObject& object, float x, float y)
        {
            Node node;
            node.id = object.graph.nextNodeId++;
            node.type = "cube";
            node.label = "Cube";
            node.x = x;
            node.y = y;

            auto addSocket = [&node, &object](
                const std::string& name,
                SocketDirection direction,
                SocketType type)
            {
                NodeSocket socket;
                socket.id = object.graph.nextSocketId++;
                socket.nodeId = node.id;
                socket.name = name;
                socket.direction = direction;
                socket.type = type;
                node.sockets.push_back(socket);
            };

            addSocket("Transform", SocketDirection::Input, SocketType::Transform);
            addSocket("Mesh", SocketDirection::Output, SocketType::Mesh);
            addSocket("Renderer", SocketDirection::Output, SocketType::Renderer);

            return node;
        }
    }

    void RegisterCubeNodeType()
    {
        RegisterNodeType(
            "cube",
            "Cube",
            CreateCubeNodeInstance
        );
    }
}
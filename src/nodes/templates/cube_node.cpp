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

            // Create output sockets matching the object's component order.
            // Each component in componentOrder gets a socket with matching name/type.
            for (size_t i = 0; i < object.componentOrder.size(); ++i)
            {
                const std::string& typeName = object.componentOrder[i];

                SocketDirection direction = SocketDirection::Output;
                SocketType socketType = SocketType::Mesh;

                if (typeName == "Transform")
                    socketType = SocketType::Transform;
                else if (typeName == "Mesh")
                    socketType = SocketType::Mesh;
                else if (typeName == "Renderer")
                    socketType = SocketType::Renderer;

                NodeSocket socket;
                socket.id = object.graph.nextSocketId++;
                socket.nodeId = node.id;
                socket.name = typeName;
                socket.direction = direction;
                socket.type = socketType;
                node.sockets.push_back(socket);
            }

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

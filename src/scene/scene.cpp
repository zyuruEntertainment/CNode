#include "scene/scene.h"

#include "nodes/node_registry.h"

#include <iostream>

namespace CNode
{
    namespace
    {
        Scene scene;
        bool nodesRegistered = false;
    }

    Scene& GetScene()
    {
        if (!nodesRegistered)
        {
            nodesRegistered = true;
            RegisterBuiltinNodes();
        }

        return scene;
    }

    SceneObject* FindSceneObject(unsigned long long id)
    {
        Scene& scene = GetScene();

        for (SceneObject& object : scene.objects)
        {
            if (object.id == id)
                return &object;
        }

        return nullptr;
    }

    SceneObject* GetSelectedSceneObject()
    {
        return FindSceneObject(GetScene().selectedObjectId);
    }

    SceneObject& CreateSceneObject(const std::string& name)
    {
        Scene& scene = GetScene();

        SceneObject object;
        object.id = scene.nextObjectId++;
        object.name = name.empty() ? "NewObject" : name;
        object.transform.scale = { 1.0f, 1.0f, 1.0f };
        object.mesh.type = "Cube";
        object.renderer.enabled = true;

        scene.objects.push_back(object);
        scene.selectedObjectId = scene.objects.back().id;

        return scene.objects.back();
    }

    void CreateCubeNode(SceneObject& object)
    {
        const auto& types = GetRegisteredNodeTypes();

        for (const RegisteredNodeType& type : types)
        {
            if (type.type == "cube")
            {
                Node node = type.factory(
                    object,
                    80.0f + static_cast<float>(object.graph.nodes.size()) * 260.0f,
                    80.0f
                );

                object.graph.nodes.push_back(node);
                object.selectedNodeId = object.graph.nodes.back().id;
                return;
            }
        }
    }

    bool RenameSceneObject(unsigned long long id, const std::string& name)
    {
        if (name.empty())
            return false;

        bool onlyWhitespace = true;

        for (char c : name)
        {
            if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
            {
                onlyWhitespace = false;
                break;
            }
        }

        if (onlyWhitespace)
            return false;

        SceneObject* object = FindSceneObject(id);

        if (!object)
            return false;

        object->name = name;
        return true;
    }
}
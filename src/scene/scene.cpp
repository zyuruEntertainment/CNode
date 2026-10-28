#include "scene/scene.h"

#include "nodes/node_registry.h"

#include <algorithm>
#include <iostream>
#include <string>

namespace CNode
{
    namespace
    {
        Scene scene;
        bool nodesRegistered = false;

        // Clipboard for copy/paste of scene objects.
        SceneObject gCopiedObject{};
        bool gHasCopiedObject = false;

        // Clipboard for copy/paste of individual component settings.
        struct ComponentClipboard
        {
            std::string type;
            Vec3 position{};
            Vec3 rotation{};
            Vec3 scale{};
            std::string meshType;
            bool rendererEnabled = false;
            float rendererColor[3] = { 0.75f, 0.75f, 0.80f };
        };
        ComponentClipboard gComponentClipboard{};
        bool gHasCopiedComponent = false;

        // Generates a name that does not collide with any existing scene object.
        std::string MakeUniqueObjectName(Scene& scene, const char* desired)
        {
            auto nameExists = [&](const std::string& name)
            {
                for (const SceneObject& object : scene.objects)
                    if (object.name == name)
                        return true;
                return false;
            };

            std::string name = desired;
            if (!nameExists(name))
                return name;

            for (int suffix = 2; ; ++suffix)
            {
                std::string candidate = name + " " + std::to_string(suffix);
                if (!nameExists(candidate))
                    return candidate;
            }
        }
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
        object.transform.position = { 0.0f, 0.0f, 0.0f };
        object.transform.rotation = { 0.0f, 0.0f, 0.0f };
        object.mesh.type = "";
        object.renderer.enabled = false;
        object.graph.nodes.clear();
        object.graph.connections.clear();

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
                object.graph.selectedNodeId = object.graph.nodes.back().id;
                object.mesh.type = "Cube";
                object.renderer.enabled = true;
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

    void DeleteSceneObject(unsigned long long id)
    {
        Scene& scene = GetScene();

        auto it = std::find_if(
            scene.objects.begin(),
            scene.objects.end(),
            [id](const SceneObject& object) { return object.id == id; });

        if (it == scene.objects.end())
            return;

        scene.objects.erase(it);

        if (scene.selectedObjectId == id)
        {
            scene.selectedObjectId =
                scene.objects.empty() ? 0 : scene.objects.front().id;
        }
    }

    void CopySceneObject(unsigned long long id)
    {
        SceneObject* object = FindSceneObject(id);

        if (!object)
            return;

        gCopiedObject = *object;
        gHasCopiedObject = true;
    }

    void PasteSceneObject()
    {
        if (!gHasCopiedObject)
            return;

        Scene& scene = GetScene();

        SceneObject object = gCopiedObject;
        object.id = scene.nextObjectId++;
        object.name = MakeUniqueObjectName(scene, (object.name + " Copy").c_str());

        scene.objects.push_back(object);
        scene.selectedObjectId = scene.objects.back().id;
    }

    bool HasCopiedObject()
    {
        return gHasCopiedObject;
    }

    void AddComponent(unsigned long long objectId, const std::string& componentType)
    {
        SceneObject* object = FindSceneObject(objectId);

        if (!object)
            return;

        Scene& scene = GetScene();

        // Don't add duplicates.
        for (const std::string& existing : scene.componentOrder)
            if (existing == componentType)
                return;

        scene.componentOrder.push_back(componentType);
    }

    void RemoveComponent(unsigned long long objectId, const std::string& componentType)
    {
        SceneObject* object = FindSceneObject(objectId);

        if (!object)
            return;

        Scene& scene = GetScene();

        auto it = std::find(
            scene.componentOrder.begin(),
            scene.componentOrder.end(),
            componentType);

        if (it == scene.componentOrder.end())
            return;

        scene.componentOrder.erase(it);

        // Clear the component's data.
        if (componentType == "Transform")
        {
            object->transform = TransformComponent{};
        }
        else if (componentType == "Mesh")
        {
            object->mesh = MeshComponent{};
        }
        else if (componentType == "Renderer")
        {
            object->renderer = RendererComponent{};
        }
    }

    void MoveComponent(unsigned long long objectId, int fromIndex, int toIndex)
    {
        (void)objectId; // Components belong to the scene, not a specific object.

        Scene& scene = GetScene();

        if (fromIndex < 0 || fromIndex >= static_cast<int>(scene.componentOrder.size()))
            return;
        if (toIndex < 0 || toIndex >= static_cast<int>(scene.componentOrder.size()))
            return;
        if (fromIndex == toIndex)
            return;

        std::string type = scene.componentOrder[fromIndex];
        scene.componentOrder.erase(scene.componentOrder.begin() + fromIndex);
        scene.componentOrder.insert(scene.componentOrder.begin() + toIndex, type);
    }

    bool HasComponent(unsigned long long objectId, const std::string& componentType)
    {
        (void)objectId;

        Scene& scene = GetScene();

        for (const std::string& existing : scene.componentOrder)
            if (existing == componentType)
                return true;

        return false;
    }

    void CopyComponent(unsigned long long objectId, const std::string& componentType)
    {
        SceneObject* object = FindSceneObject(objectId);

        if (!object)
            return;

        gComponentClipboard.type = componentType;

        if (componentType == "Transform")
        {
            gComponentClipboard.position = object->transform.position;
            gComponentClipboard.rotation = object->transform.rotation;
            gComponentClipboard.scale = object->transform.scale;
        }
        else if (componentType == "Mesh")
        {
            gComponentClipboard.meshType = object->mesh.type;
        }
        else if (componentType == "Renderer")
        {
            gComponentClipboard.rendererEnabled = object->renderer.enabled;
            gComponentClipboard.rendererColor[0] = object->renderer.color[0];
            gComponentClipboard.rendererColor[1] = object->renderer.color[1];
            gComponentClipboard.rendererColor[2] = object->renderer.color[2];
        }

        gHasCopiedComponent = true;
    }

    void PasteComponent(unsigned long long objectId, const std::string& componentType)
    {
        if (!gHasCopiedComponent)
            return;

        SceneObject* object = FindSceneObject(objectId);

        if (!object)
            return;

        if (gComponentClipboard.type != componentType)
            return;

        if (componentType == "Transform")
        {
            object->transform.position = gComponentClipboard.position;
            object->transform.rotation = gComponentClipboard.rotation;
            object->transform.scale = gComponentClipboard.scale;
        }
        else if (componentType == "Mesh")
        {
            object->mesh.type = gComponentClipboard.meshType;
        }
        else if (componentType == "Renderer")
        {
            object->renderer.enabled = gComponentClipboard.rendererEnabled;
            object->renderer.color[0] = gComponentClipboard.rendererColor[0];
            object->renderer.color[1] = gComponentClipboard.rendererColor[1];
            object->renderer.color[2] = gComponentClipboard.rendererColor[2];
        }
    }

    bool HasCopiedComponent()
    {
        return gHasCopiedComponent;
    }
}

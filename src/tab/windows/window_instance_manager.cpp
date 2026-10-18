#include "tab/windows/window_instance_manager.h"
#include "tab/windows/window_registry.h"

#include <algorithm>
#include <limits>
#include <utility>

namespace CNode
{
    namespace
    {
        std::list<WindowInstance>& GetInstanceStorage()
        {
            static std::list<WindowInstance> instances;
            return instances;
        }

        unsigned long long& GetNextInstanceID()
        {
            static unsigned long long nextID = 1;
            return nextID;
        }
    }

    WindowInstance* AddWindow(const char* typeID)
    {
        if (typeID == nullptr || *typeID == '\0')
            return nullptr;

        const auto& types = GetRegisteredWindows();

        const auto type = std::find_if(
            types.begin(),
            types.end(),
            [typeID](const WindowType& item)
            {
                return item.id == typeID;
            }
        );

        if (type == types.end())
            return nullptr;

        auto& nextID = GetNextInstanceID();

        if (nextID == 0 ||
            nextID == std::numeric_limits<unsigned long long>::max())
        {
            return nullptr;
        }

        const unsigned long long id = nextID++;

        std::string uniqueTitle =
            type->name +
            "###" +
            type->id +
            "_" +
            std::to_string(id);

        auto& instances = GetInstanceStorage();

        instances.push_back({
            id,
            type->id,
            std::move(uniqueTitle),
            true
            });

        return &instances.back();
    }

    std::list<WindowInstance>& GetWindowInstances()
    {
        return GetInstanceStorage();
    }

    void RemoveClosedWindowInstances()
    {
        GetInstanceStorage().remove_if(
            [](const WindowInstance& instance)
            {
                return !instance.open;
            }
        );
    }
}


#include "tab/windows/window_id.h"

#include <string>
#include <unordered_set>

namespace CNode
{
    namespace
    {
        std::unordered_set<std::string>& GetWindowTypeIDs()
        {
            static std::unordered_set<std::string> ids;
            return ids;
        }

        std::unordered_set<std::string>& GetWindowAttributeIDs()
        {
            static std::unordered_set<std::string> ids;
            return ids;
        }
    }

    bool IsValidWindowID(const char* id)
    {
        if (id == nullptr || *id == '\0')
            return false;

        for (const char* current = id; *current != '\0'; ++current)
        {
            const char c = *current;

            const bool valid =
                (c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                c == '_' ||
                c == '-';

            if (!valid)
                return false;
        }

        return true;
    }

    bool RegisterWindowTypeID(const char* id)
    {
        if (!IsValidWindowID(id))
            return false;

        return GetWindowTypeIDs().insert(id).second;
    }

    bool RegisterWindowAttributeID(const char* id)
    {
        if (!IsValidWindowID(id))
            return false;

        return GetWindowAttributeIDs().insert(id).second;
    }
}

#include "tab/windows/project_setting/project_setting_data.h"

namespace CNode
{
    namespace
    {
        ProjectSettings settings;
    }

    ProjectSettings& GetProjectSettings()
    {
        return settings;
    }
}
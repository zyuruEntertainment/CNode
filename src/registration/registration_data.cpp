#include "registration/registration_data.h"
#include "registration/registration_registry.h"
#include "tab/windows/window_registry.h"

namespace CNode
{
    void Windows()
    {
        RegisterAllFeatures();
        RegisterWindowAttributes();
    }
}

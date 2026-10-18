#include "scene/window/editor.h"
#include "registration/registration_data.h"
#include "registration/registration_registry.h"

#include <iostream>

int main()
{
    CNode::EditorWindow editor;

    if (!editor.Initialize())
    {
        std::cerr << "CNode: Application startup failed.\n";
        return 1;
    }

    // Panggil seluruh fungsi registrasi fitur.
    CNode::RegisterAllFeatures();

    // Proses metadata khusus window.
    CNode::Windows();

    editor.Run();

    editor.Shutdown();

    return 0;
}
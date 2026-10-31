#include "tab/windows/inspector/inspector_add_component.h"

#include "imgui.h"

#include <filesystem>
#include <cctype>
#include <algorithm>
#include <vector>
#include <string>

namespace CNode
{
    void DrawAddComponentPopup(SceneObject& object)
    {
        if (ImGui::BeginPopup("InspectorAddComponent"))
        {
            std::vector<std::string> availableTypes;
            namespace fs = std::filesystem;
            fs::path componentsDir = "include/components";
            if (fs::exists(componentsDir) && fs::is_directory(componentsDir))
            {
                for (const auto& entry : fs::directory_iterator(componentsDir))
                {
                    if (entry.is_directory())
                    {
                        std::string name = entry.path().filename().string();
                        if (!name.empty())
                            name[0] = static_cast<char>(
                                std::toupper(static_cast<unsigned char>(name[0])));
                        availableTypes.push_back(name);
                    }
                }
            }
            std::sort(availableTypes.begin(), availableTypes.end());

            for (const auto& type : availableTypes)
            {
                bool alreadyHas = HasComponent(object.id, type);
                if (!alreadyHas && ImGui::MenuItem(type.c_str()))
                {
                    AddComponent(object.id, type);
                }
            }
            ImGui::EndPopup();
        }
    }
}

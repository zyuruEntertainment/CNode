#include "registration/registration.h"
#include "registration/registration_data.h"
#include "registration/window_docking.h"

#include "tab/windows/window_registry.h"
#include "tab/windows/window_instance_manager.h"

#include "imgui.h"

#include <algorithm>
#include <cstdlib>
#include <ctime>

namespace CNode
{
    void DrawRegisteredWindows()
    {
        static const bool randomSeeded = []
            {
                std::srand(static_cast<unsigned int>(std::time(nullptr)));
                return true;
            }();

        (void)randomSeeded;

        Windows();

        const auto& types = GetRegisteredWindows();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Windows"))
            {
                for (const WindowType& type : types)
                {
                    ImGui::PushID(type.id.c_str());

                    if (ImGui::MenuItem(type.name.c_str()))
                    {
                        AddWindow(type.id.c_str());
                    }

                    ImGui::PopID();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        DrawEditorDockspace();

        auto& instances = GetWindowInstances();

        for (WindowInstance& instance : instances)
        {
            if (!instance.open)
                continue;

            auto type = std::find_if(
                types.begin(),
                types.end(),
                [&instance](const WindowType& item)
                {
                    return item.id == instance.typeID;
                }
            );

            if (type == types.end() || !type->draw)
                continue;

            ImGuiViewport* viewport = ImGui::GetMainViewport();

            const float maxX =
                (viewport->WorkSize.x > 850.0f)
                ? viewport->WorkSize.x - 800.0f
                : 50.0f;

            const float maxY =
                (viewport->WorkSize.y > 550.0f)
                ? viewport->WorkSize.y - 500.0f
                : 50.0f;

            const float randomX =
                static_cast<float>(std::rand()) /
                static_cast<float>(RAND_MAX);

            const float randomY =
                static_cast<float>(std::rand()) /
                static_cast<float>(RAND_MAX);

            const ImVec2 initialPosition(
                viewport->WorkPos.x + randomX * maxX,
                viewport->WorkPos.y + randomY * maxY
            );

            ImGui::SetNextWindowDockID(
                0,
                ImGuiCond_FirstUseEver
            );

            ImGui::SetNextWindowPos(
                initialPosition,
                ImGuiCond_FirstUseEver
            );

            ImGui::SetNextWindowSize(
                ImVec2(800.0f, 500.0f),
                ImGuiCond_FirstUseEver
            );

            type->draw(
                instance.title.c_str(),
                &instance.open
            );
        }

        RemoveClosedWindowInstances();
    }
}
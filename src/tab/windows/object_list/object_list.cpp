#include "tab/windows/object_list/object_list.h"
#include "scene/scene.h"
#include "ui/add_button.h"

#include "imgui.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>

namespace CNode
{
    namespace
    {
        bool gRenaming = false;
        unsigned long long gRenamingId = 0;
        char gRenameBuffer[256] = {};
        bool gItemHovered = false;
    }

    static void StartRename(SceneObject& object)
    {
        gRenaming = true;
        gRenamingId = object.id;
        std::string name = object.name;
        std::snprintf(gRenameBuffer, sizeof(gRenameBuffer), "%s", name.c_str());
    }

    // Generates a name that does not collide with any existing scene object.
    // Dear ImGui derives widget IDs from labels, so duplicate names (e.g. two
    // objects both called "NewObject") produce conflicting Selectable IDs.
    static std::string MakeUniqueObjectName(Scene& scene, const char* desired)
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

        // Append " 2", " 3", ... until a free name is found.
        for (int suffix = 2; ; ++suffix)
        {
            std::string candidate = name + " " + std::to_string(suffix);
            if (!nameExists(candidate))
                return candidate;
        }
    }

    static void CreateNewObject(Scene& scene)
    {
        SceneObject& object = CreateSceneObject(MakeUniqueObjectName(scene, "NewObject"));
        scene.selectedObjectId = object.id;
        StartRename(object);
    }

    void DrawObjectList(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        Scene& scene = GetScene();

        gItemHovered = false;

        UI::AddButton("ObjectListContextMenu", "+ Add Object");

        if (ImGui::IsWindowHovered() &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
            !gItemHovered)
        {
            ImGui::OpenPopup("ObjectListContextMenu");
        }

        if (ImGui::BeginPopup("ObjectListContextMenu"))
        {
            if (ImGui::MenuItem("Add Object"))
            {
                CreateNewObject(scene);
                ImGui::SetWindowFocus(title);
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        for (SceneObject& object : scene.objects)
        {
            const bool selected = (scene.selectedObjectId == object.id);

            if (gRenaming && gRenamingId == object.id)
            {
                if (ImGui::InputText(
                        "##rename",
                        gRenameBuffer,
                        sizeof(gRenameBuffer),
                        ImGuiInputTextFlags_EnterReturnsTrue |
                        ImGuiInputTextFlags_AutoSelectAll))
                {
                    std::string newName = gRenameBuffer;

                    bool onlyWhitespace = true;

                    for (char c : newName)
                    {
                        if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
                        {
                            onlyWhitespace = false;
                            break;
                        }
                    }

                    if (!onlyWhitespace)
                        RenameSceneObject(object.id, newName);

                    gRenaming = false;
                    gRenamingId = 0;
                }

                if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                {
                    gRenaming = false;
                    gRenamingId = 0;
                }
            }
            else
            {
                if (ImGui::Selectable(object.name.c_str(), selected))
                {
                    scene.selectedObjectId = object.id;
                }

                if (ImGui::IsItemHovered())
                    gItemHovered = true;

                if (selected && ImGui::IsKeyPressed(ImGuiKey_F2))
                    StartRename(object);

                if (ImGui::IsItemHovered() &&
                    ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    StartRename(object);
            }
        }

        ImGui::End();
    }
}
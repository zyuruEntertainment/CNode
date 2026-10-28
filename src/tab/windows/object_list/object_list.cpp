#include "tab/windows/object_list/object_list.h"
#include "scene/scene.h"

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

    static void CreateNewObject(Scene& scene)
    {
        SceneObject& object = CreateSceneObject("NewObject");
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

        if (ImGui::Button("+"))
            ImGui::OpenPopup("ObjectListContextMenu");

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
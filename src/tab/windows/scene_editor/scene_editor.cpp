#include "tab/windows/scene_editor/scene_editor.h"
#include "tab/windows/scene_editor/scene_camera.h"
#include "tab/windows/scene_editor/scene_renderer.h"
#include "tab/windows/scene_editor/navigation/orbit_navigation.h"

#include "imgui.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

namespace CNode
{
    namespace
    {
        struct SceneEditorState
        {
            SceneCamera camera;
            SceneRenderer renderer;
            OrbitNavigation orbitNavigation;
        };

        std::unordered_map<
            std::string,
            std::unique_ptr<SceneEditorState>
        > sceneEditors;

        SceneEditorState* GetSceneEditorState(const char* title)
        {
            if (!title || !*title)
                return nullptr;

            auto& state = sceneEditors[title];

            if (!state)
                state = std::make_unique<SceneEditorState>();

            return state.get();
        }
    }

    void DrawSceneEditor(const char* title, bool* open)
    {
        if (!title || !*title)
            return;

        if (!ImGui::Begin(title, open))
        {
            ImGui::End();

            if (open && !*open)
                sceneEditors.erase(title);

            return;
        }

        const ImVec2 available = ImGui::GetContentRegionAvail();

        if (available.x <= 1.0f || available.y <= 1.0f)
        {
            ImGui::End();
            return;
        }

        SceneEditorState* state = GetSceneEditorState(title);

        if (!state)
        {
            ImGui::TextUnformatted(
                "Failed to create the scene editor state."
            );

            ImGui::End();
            return;
        }

        if (!state->renderer.IsInitialized() &&
            !state->renderer.Initialize())
        {
            ImGui::TextUnformatted(
                "Failed to initialize the 3D scene renderer."
            );

            ImGui::End();
            return;
        }

        // Gunakan ukuran area 3D milik jendela ini.
        const int width = std::max(
            1,
            static_cast<int>(available.x)
        );

        const int height = std::max(
            1,
            static_cast<int>(available.y)
        );

        state->renderer.Render(
            state->camera,
            width,
            height
        );

        const unsigned int texture =
            state->renderer.GetTexture();

        if (texture != 0)
        {
            ImGui::Image(
                (ImTextureID)(intptr_t)texture,
                ImVec2(
                    static_cast<float>(width),
                    static_cast<float>(height)
                ),
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );

            state->orbitNavigation.Update(
                state->camera,
                ImGui::IsItemHovered()
            );
        }

        ImGui::End();
    }

    void ShutdownSceneEditor()
    {
        for (auto& entry : sceneEditors)
        {
            if (entry.second)
                entry.second->renderer.Shutdown();
        }

        sceneEditors.clear();
    }
}
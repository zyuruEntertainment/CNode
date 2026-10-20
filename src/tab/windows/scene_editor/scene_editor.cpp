#include "tab/windows/scene_editor/scene_editor.h"
#include "tab/windows/scene_editor/scene_camera.h"
#include "tab/windows/scene_editor/scene_renderer.h"
#include "tab/windows/scene_editor/navigation/orbit_navigation.h"
#include "tab/windows/scene_editor/navigation/zoom_navigation.h"
#include "tab/windows/scene_editor/navigation/pan_navigation.h"
#include "tab/windows/scene_editor/navigation/free_look_navigation.h"

#include "imgui.h"

#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
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
            ZoomNavigation zoomNavigation;
            PanNavigation panNavigation;
            FreeLookNavigation freeLookNavigation;
        };

        std::unordered_map<
            std::string,
            std::unique_ptr<SceneEditorState>
        > sceneEditors;

        SceneEditorState* GetSceneEditorState(
            const char* title
        )
        {
            if (!title || !*title)
                return nullptr;

            auto& state = sceneEditors[title];

            if (!state)
            {
                state =
                    std::make_unique<SceneEditorState>();
            }

            return state.get();
        }
    }

    void DrawSceneEditor(
        const char* title,
        bool* open
    )
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

        SceneEditorState* state =
            GetSceneEditorState(title);

        if (!state)
        {
            ImGui::TextUnformatted(
                "Failed to create the scene editor state."
            );

            ImGui::End();
            return;
        }

        const ImVec2 available =
            ImGui::GetContentRegionAvail();

        if (available.x <= 1.0f ||
            available.y <= 1.0f)
        {
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

        const float displayWidth = available.x;
        const float displayHeight = available.y;

        const ImGuiIO& io = ImGui::GetIO();

        const float scaleX =
            std::max(io.DisplayFramebufferScale.x, 1.0f);

        const float scaleY =
            std::max(io.DisplayFramebufferScale.y, 1.0f);

        const int renderWidth = std::max(
            1,
            static_cast<int>(
                std::lround(displayWidth * scaleX)
                )
        );

        const int renderHeight = std::max(
            1,
            static_cast<int>(
                std::lround(displayHeight * scaleY)
                )
        );

        state->renderer.Render(
            state->camera,
            renderWidth,
            renderHeight
        );

        const unsigned int texture =
            state->renderer.GetTexture();

        if (texture != 0)
        {
            ImGui::Image(
                (ImTextureID)(intptr_t)texture,
                ImVec2(
                    displayWidth,
                    displayHeight
                ),
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );

            GLFWwindow* window =
                glfwGetCurrentContext();

            const bool viewportHovered =
                ImGui::IsItemHovered();

            const bool freeLookActive =
                state->freeLookNavigation.Update(
                    state->camera,
                    window,
                    viewportHovered
                );

            const bool panActive =
                state->panNavigation.Update(
                    state->camera,
                    window,
                    viewportHovered
                );

            if (!panActive)
            {
                state->orbitNavigation.Update(
                    state->camera,
                    window,
                    viewportHovered
                );
            }

            state->zoomNavigation.Update(
                state->camera,
                viewportHovered
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

#include "tab/windows/scene_editor/scene_editor.h"
#include "tab/windows/scene_editor/scene_camera.h"
#include "tab/windows/scene_editor/scene_renderer.h"
#include "tab/windows/scene_editor/navigation/orbit_navigation.h"
#include "tab/windows/scene_editor/navigation/zoom_navigation.h"
#include "tab/windows/scene_editor/navigation/pan_navigation.h"
#include "tab/windows/scene_editor/navigation/free_look_navigation.h"
#include "tab/windows/scene_editor/ui/orbit_gizmo.h"
#include "tab/windows/scene_editor/ui/pan_view_controller.h"

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
            OrbitGizmo orbitGizmo;
            PanViewController panViewController;
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

        void DestroySceneEditorState(const char* title)
        {
            if (!title || !*title)
                return;

            const auto iterator = sceneEditors.find(title);

            if (iterator == sceneEditors.end())
                return;

            if (iterator->second)
            {
                iterator->second->panViewController.Shutdown();
                iterator->second->renderer.Shutdown();
            }

            sceneEditors.erase(iterator);
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
                DestroySceneEditorState(title);

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
            const ImVec2 viewportMin =
                ImGui::GetCursorScreenPos();

            ImGui::Image(
                (ImTextureID)(intptr_t)texture,
                ImVec2(displayWidth, displayHeight),
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );

            const ImVec2 viewportMax(
                viewportMin.x + displayWidth,
                viewportMin.y + displayHeight
            );

            state->orbitGizmo.Draw(
                state->camera,
                viewportMin,
                viewportMax
            );

            const bool panControllerActive =
                state->panViewController.Draw(
                    viewportMin,
                    viewportMax
                );

            GLFWwindow* window =
                glfwGetCurrentContext();

            const bool viewportHovered =
                ImGui::IsMouseHoveringRect(
                    viewportMin,
                    viewportMax
                );

            const bool freeLookActive =
                state->freeLookNavigation.Update(
                    state->camera,
                    window,
                    viewportHovered
                );

            if (!freeLookActive)
            {
                const bool panActive =
                    state->panNavigation.Update(
                        state->camera,
                        window,
                        viewportHovered,
                        panControllerActive
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
        }

        ImGui::End();
    }

    void ShutdownSceneEditor()
    {
        for (auto& entry : sceneEditors)
        {
            if (entry.second)
            {
                entry.second->panViewController.Shutdown();
                entry.second->renderer.Shutdown();
            }
        }

        sceneEditors.clear();
    }
}
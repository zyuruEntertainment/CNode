#include "tab/windows/scene_editor/scene_editor.h"
#include "tab/windows/scene_editor/scene_camera.h"
#include "tab/windows/scene_editor/scene_renderer.h"

#include "imgui.h"

#include <algorithm>
#include <cstdint>

namespace CNode
{
    namespace
    {
        SceneCamera sceneCamera;
        SceneRenderer sceneRenderer;
    }

    void DrawSceneEditor(const char* title, bool* open)
    {
        if (!ImGui::Begin(title, open))
        {
            ImGui::End();
            return;
        }

        const ImVec2 available = ImGui::GetContentRegionAvail();

        const int width = static_cast<int>(
            std::max(available.x, 1.0f)
            );

        const int height = static_cast<int>(
            std::max(available.y, 1.0f)
            );

        if (available.x <= 1.0f || available.y <= 1.0f)
        {
            ImGui::End();
            return;
        }

        if (!sceneRenderer.IsInitialized() &&
            !sceneRenderer.Initialize())
        {
            ImGui::TextUnformatted(
                "Failed to initialize the 3D scene renderer."
            );

            ImGui::End();
            return;
        }

        sceneRenderer.Render(
            sceneCamera,
            width,
            height
        );

        const unsigned int texture =
            sceneRenderer.GetTexture();

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
        }

        ImGui::End();
    }

    void ShutdownSceneEditor()
    {
        sceneRenderer.Shutdown();
    }
}
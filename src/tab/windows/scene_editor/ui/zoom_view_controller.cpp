#include "tab/windows/scene_editor/ui/zoom_view_controller.h"

#include "core/input/cursor_visibility.h"

#define NOMINMAX
#include <glad/glad.h>
#include <stb_image.h>

#include <cstdint>

namespace CNode
{
    bool ZoomViewController::Initialize()
    {
        if (texture != 0)
            return true;

        int width = 0;
        int height = 0;
        int channels = 0;

        unsigned char* pixels = stbi_load(
            "assets/img/Navigation_UI/Zoom_logo.png",
            &width,
            &height,
            &channels,
            4
        );

        if (!pixels)
            return false;

        GLint previousTexture = 0;
        GLint previousAlignment = 4;

        glGetIntegerv(
            GL_TEXTURE_BINDING_2D,
            &previousTexture
        );

        glGetIntegerv(
            GL_UNPACK_ALIGNMENT,
            &previousAlignment
        );

        GLuint newTexture = 0;

        glGenTextures(1, &newTexture);

        if (newTexture != 0)
        {
            glBindTexture(GL_TEXTURE_2D, newTexture);

            glPixelStorei(
                GL_UNPACK_ALIGNMENT,
                1
            );

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA8,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                pixels
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_LINEAR
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_LINEAR
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_S,
                GL_CLAMP_TO_EDGE
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_WRAP_T,
                GL_CLAMP_TO_EDGE
            );
        }

        glBindTexture(
            GL_TEXTURE_2D,
            static_cast<GLuint>(previousTexture)
        );

        glPixelStorei(
            GL_UNPACK_ALIGNMENT,
            previousAlignment
        );

        stbi_image_free(pixels);

        if (newTexture == 0)
            return false;

        texture = newTexture;
        initialized = true;

        return true;
    }

    bool ZoomViewController::Draw(
        GLFWwindow* window,
        const ImVec2& viewportMin,
        const ImVec2& viewportMax
    )
    {
        constexpr float radius = 23.0f;
        constexpr float buttonSize = radius * 2.0f;
        constexpr float padding = 10.0f;
        constexpr int startupFramesToIgnore = 2;

        dragDeltaY = 0.0f;

        const ImVec2 center(
            viewportMax.x - radius - 12.0f,
            (viewportMin.y + viewportMax.y) * 0.5f
            + radius * 2.0f + 8.0f
        );

        if (center.x - radius < viewportMin.x ||
            center.y - radius < viewportMin.y ||
            center.y + radius > viewportMax.y)
        {
            if (cursorCaptured)
            {
                CursorVisibility::SetCaptured(
                    capturedWindow,
                    false
                );

                capturedWindow = nullptr;
                cursorCaptured = false;
            }

            ignoreMouseDeltaFrames = 0;

            return false;
        }

        Initialize();

        ImGui::SetCursorScreenPos(
            ImVec2(
                center.x - radius,
                center.y - radius
            )
        );

        ImGui::InvisibleButton(
            "##ZoomViewController",
            ImVec2(buttonSize, buttonSize)
        );

        const bool hovered = ImGui::IsItemHovered();

        const bool active =
            ImGui::IsItemActive() &&
            ImGui::IsMouseDown(ImGuiMouseButton_Left);

        if (active && !cursorCaptured && window)
        {
            CursorVisibility::SetCaptured(
                window,
                true
            );

            capturedWindow = window;
            cursorCaptured = true;
            ignoreMouseDeltaFrames = startupFramesToIgnore;
        }
        else if (!active && cursorCaptured)
        {
            CursorVisibility::SetCaptured(
                capturedWindow,
                false
            );

            capturedWindow = nullptr;
            cursorCaptured = false;
            ignoreMouseDeltaFrames = 0;
        }

        if (active && cursorCaptured)
        {
            if (ignoreMouseDeltaFrames > 0)
            {
                --ignoreMouseDeltaFrames;
            }
            else
            {
                dragDeltaY = ImGui::GetIO().MouseDelta.y;
            }
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        const ImU32 background = active
            ? IM_COL32(60, 100, 150, 245)
            : hovered
            ? IM_COL32(48, 52, 62, 245)
            : IM_COL32(29, 32, 39, 235);

        const ImU32 border = active
            ? IM_COL32(105, 175, 255, 255)
            : hovered
            ? IM_COL32(130, 145, 165, 255)
            : IM_COL32(82, 88, 99, 230);

        drawList->AddCircleFilled(
            center,
            radius,
            background,
            48
        );

        drawList->AddCircle(
            center,
            radius,
            border,
            48,
            active ? 2.0f : 1.4f
        );

        if (texture != 0)
        {
            const ImVec2 imageMin(
                center.x - radius + padding,
                center.y - radius + padding
            );

            const ImVec2 imageMax(
                center.x + radius - padding,
                center.y + radius - padding
            );

            drawList->AddImage(
                (ImTextureID)(intptr_t)texture,
                imageMin,
                imageMax
            );
        }

        if (hovered)
            ImGui::SetTooltip("Zoom View");

        return active;
    }

    float ZoomViewController::GetDragDeltaY() const
    {
        return dragDeltaY;
    }

    void ZoomViewController::Shutdown()
    {
        if (cursorCaptured)
        {
            CursorVisibility::SetCaptured(
                capturedWindow,
                false
            );

            capturedWindow = nullptr;
            cursorCaptured = false;
        }

        dragDeltaY = 0.0f;
        ignoreMouseDeltaFrames = 0;

        if (texture != 0)
        {
            const GLuint textureId =
                static_cast<GLuint>(texture);

            glDeleteTextures(1, &textureId);
            texture = 0;
        }

        initialized = false;
    }
}
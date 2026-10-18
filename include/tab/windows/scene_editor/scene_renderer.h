#pragma once

#include "tab/windows/scene_editor/scene_camera.h"

namespace CNode
{
    class SceneRenderer
    {
    public:
        bool Initialize();
        void Render(
            const SceneCamera& camera,
            int width,
            int height
        );

        void Shutdown();

        unsigned int GetTexture() const;
        bool IsInitialized() const;

    private:
        bool CreateFramebuffer(int width, int height);
        bool CreateGrid();
        bool CreateShader();

        unsigned int framebuffer = 0;
        unsigned int colorTexture = 0;
        unsigned int depthBuffer = 0;

        unsigned int vertexArray = 0;
        unsigned int vertexBuffer = 0;
        unsigned int shaderProgram = 0;

        int framebufferWidth = 0;
        int framebufferHeight = 0;
        int vertexCount = 0;

        bool initialized = false;
    };
}
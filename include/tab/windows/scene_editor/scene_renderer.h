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
        bool CreateShader();
        bool CreateGrid(
            const SceneCamera& camera,
            int width,
            int height
        );
        bool CreateAxis(const SceneCamera& camera);
        bool UpdateGrid(
            const SceneCamera& camera,
            int width,
            int height
        );
        bool UpdateAxis(const SceneCamera& camera);
        bool CreateLineBuffer(
            const void* data,
            int vertexCount,
            unsigned int& vertexArray,
            unsigned int& vertexBuffer,
            unsigned int usage = 0x88E8
        );
        bool CreateFramebuffer(int width, int height);

        unsigned int framebuffer = 0;
        unsigned int colorTexture = 0;
        unsigned int depthBuffer = 0;

        unsigned int shaderProgram = 0;

        unsigned int gridVertexArray = 0;
        unsigned int gridVertexBuffer = 0;
        int gridVertexCount = 0;

        unsigned int gridMajorVertexArray = 0;
        unsigned int gridMajorVertexBuffer = 0;
        int gridMajorVertexCount = 0;

        unsigned int axisVertexArray = 0;
        unsigned int axisVertexBuffer = 0;
        int axisVertexCount = 0;

        int framebufferWidth = 0;
        int framebufferHeight = 0;

        bool initialized = false;
    };
}
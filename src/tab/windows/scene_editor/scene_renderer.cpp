#include "tab/windows/scene_editor/scene_renderer.h"

#define NOMINMAX
#include <glad/gl.h>

#include <cstddef>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace CNode
{
    namespace
    {
        struct GridVertex
        {
            float position[3];
            float color[3];
        };

        void AddLine(
            std::vector<GridVertex>& vertices,
            float x1, float y1, float z1,
            float x2, float y2, float z2,
            float r, float g, float b
        )
        {
            vertices.push_back({
                { x1, y1, z1 },
                { r, g, b }
                });

            vertices.push_back({
                { x2, y2, z2 },
                { r, g, b }
                });
        }

        unsigned int CompileShader(
            unsigned int type,
            const char* source
        )
        {
            const unsigned int shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);

            int success = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                char log[1024] = {};
                glGetShaderInfoLog(
                    shader,
                    sizeof(log),
                    nullptr,
                    log
                );

                std::cerr
                    << "CNode: Shader compilation failed: "
                    << log << '\n';

                glDeleteShader(shader);
                return 0;
            }

            return shader;
        }
    }

    bool SceneRenderer::Initialize()
    {
        if (initialized)
            return true;

        if (!CreateShader())
            return false;

        if (!CreateGrid())
        {
            Shutdown();
            return false;
        }

        glGenFramebuffers(1, &framebuffer);

        if (framebuffer == 0)
        {
            Shutdown();
            return false;
        }

        initialized = true;
        return true;
    }

    bool SceneRenderer::CreateShader()
    {
        const char* vertexSource = R"(
            #version 330 core

            layout (location = 0) in vec3 aPosition;
            layout (location = 1) in vec3 aColor;

            uniform mat4 uViewProjection;

            out vec3 vertexColor;

            void main()
            {
                gl_Position = uViewProjection * vec4(aPosition, 1.0);
                vertexColor = aColor;
            }
        )";

        const char* fragmentSource = R"(
            #version 330 core

            in vec3 vertexColor;
            out vec4 FragColor;

            void main()
            {
                FragColor = vec4(vertexColor, 1.0);
            }
        )";

        const unsigned int vertexShader =
            CompileShader(GL_VERTEX_SHADER, vertexSource);

        if (!vertexShader)
            return false;

        const unsigned int fragmentShader =
            CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        if (!fragmentShader)
        {
            glDeleteShader(vertexShader);
            return false;
        }

        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        int success = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

        if (!success)
        {
            char log[1024] = {};

            glGetProgramInfoLog(
                shaderProgram,
                sizeof(log),
                nullptr,
                log
            );

            std::cerr
                << "CNode: Shader linking failed: "
                << log << '\n';

            glDeleteProgram(shaderProgram);
            shaderProgram = 0;

            return false;
        }

        return true;
    }

    bool SceneRenderer::CreateGrid()
    {
        std::vector<GridVertex> vertices;

        constexpr int gridSize = 10;
        constexpr float gridColor = 0.25f;
        constexpr float axisColor = 0.65f;

        for (int i = -gridSize; i <= gridSize; ++i)
        {
            const float position = static_cast<float>(i);

            if (i == 0)
            {
                AddLine(
                    vertices,
                    position, 0.0f, -gridSize,
                    position, 0.0f, gridSize,
                    0.20f, 0.40f, axisColor
                );

                AddLine(
                    vertices,
                    -gridSize, 0.0f, position,
                    gridSize, 0.0f, position,
                    axisColor, 0.25f, 0.25f
                );
            }
            else
            {
                AddLine(
                    vertices,
                    position, 0.0f, -gridSize,
                    position, 0.0f, gridSize,
                    gridColor, gridColor, gridColor
                );

                AddLine(
                    vertices,
                    -gridSize, 0.0f, position,
                    gridSize, 0.0f, position,
                    gridColor, gridColor, gridColor
                );
            }
        }

        vertexCount = static_cast<int>(vertices.size());

        glGenVertexArrays(1, &vertexArray);
        glGenBuffers(1, &vertexBuffer);

        if (!vertexArray || !vertexBuffer)
            return false;

        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(
                vertices.size() * sizeof(GridVertex)
                ),
            vertices.data(),
            GL_STATIC_DRAW
        );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(GridVertex),
            reinterpret_cast<void*>(0)
        );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(GridVertex),
            reinterpret_cast<void*>(
                offsetof(GridVertex, color)
                )
        );

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    bool SceneRenderer::CreateFramebuffer(int width, int height)
    {
        width = std::max(width, 1);
        height = std::max(height, 1);

        if (framebufferWidth == width &&
            framebufferHeight == height &&
            colorTexture != 0 &&
            depthBuffer != 0)
        {
            return true;
        }

        int previousFramebuffer = 0;
        glGetIntegerv(
            GL_FRAMEBUFFER_BINDING,
            &previousFramebuffer
        );

        if (colorTexture)
            glDeleteTextures(1, &colorTexture);

        if (depthBuffer)
            glDeleteRenderbuffers(1, &depthBuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            nullptr
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

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D,
            colorTexture,
            0
        );

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

        glRenderbufferStorage(
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,
            width,
            height
        );

        glFramebufferRenderbuffer(
            GL_FRAMEBUFFER,
            GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER,
            depthBuffer
        );

        const bool complete =
            glCheckFramebufferStatus(GL_FRAMEBUFFER) ==
            GL_FRAMEBUFFER_COMPLETE;

        glBindFramebuffer(
            GL_FRAMEBUFFER,
            static_cast<unsigned int>(previousFramebuffer)
        );

        framebufferWidth = width;
        framebufferHeight = height;

        if (!complete)
        {
            std::cerr
                << "CNode: Scene framebuffer is incomplete.\n";

            return false;
        }

        return true;
    }

    void SceneRenderer::Render(
        const SceneCamera& camera,
        int width,
        int height
    )
    {
        if (!initialized || width <= 0 || height <= 0)
            return;

        if (!CreateFramebuffer(width, height))
            return;

        int previousFramebuffer = 0;
        int previousViewport[4] = {};

        glGetIntegerv(
            GL_FRAMEBUFFER_BINDING,
            &previousFramebuffer
        );

        glGetIntegerv(GL_VIEWPORT, previousViewport);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        glClearColor(0.075f, 0.085f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float viewProjection[16] = {};

        camera.GetViewProjection(
            static_cast<float>(width),
            static_cast<float>(height),
            viewProjection
        );

        glUseProgram(shaderProgram);

        const int matrixLocation =
            glGetUniformLocation(
                shaderProgram,
                "uViewProjection"
            );

        glUniformMatrix4fv(
            matrixLocation,
            1,
            GL_FALSE,
            viewProjection
        );

        glBindVertexArray(vertexArray);
        glDrawArrays(GL_LINES, 0, vertexCount);
        glBindVertexArray(0);

        glUseProgram(0);

        glBindFramebuffer(
            GL_FRAMEBUFFER,
            static_cast<unsigned int>(previousFramebuffer)
        );

        glViewport(
            previousViewport[0],
            previousViewport[1],
            previousViewport[2],
            previousViewport[3]
        );
    }

    unsigned int SceneRenderer::GetTexture() const
    {
        return colorTexture;
    }

    bool SceneRenderer::IsInitialized() const
    {
        return initialized;
    }

    void SceneRenderer::Shutdown()
    {
        if (vertexBuffer)
            glDeleteBuffers(1, &vertexBuffer);

        if (vertexArray)
            glDeleteVertexArrays(1, &vertexArray);

        if (shaderProgram)
            glDeleteProgram(shaderProgram);

        if (colorTexture)
            glDeleteTextures(1, &colorTexture);

        if (depthBuffer)
            glDeleteRenderbuffers(1, &depthBuffer);

        if (framebuffer)
            glDeleteFramebuffers(1, &framebuffer);

        vertexBuffer = 0;
        vertexArray = 0;
        shaderProgram = 0;
        colorTexture = 0;
        depthBuffer = 0;
        framebuffer = 0;

        framebufferWidth = 0;
        framebufferHeight = 0;
        vertexCount = 0;

        initialized = false;
    }
}
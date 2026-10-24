#include "tab/windows/scene_editor/scene_renderer.h"
#include "tab/windows/scene_editor/scene_grid.h"
#include "tab/windows/scene_editor/scene_axis.h"

#define NOMINMAX
#include <glad/glad.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

namespace CNode
{
    namespace
    {
        unsigned int CompileShader(
            unsigned int type,
            const char* source)
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

        if (!CreateShader() ||
            !CreateGrid() ||
            !CreateAxis())
        {
            Shutdown();
            return false;
        }

        glGenFramebuffers(1, &framebuffer);

        if (!framebuffer)
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
                gl_Position =
                    uViewProjection * vec4(aPosition, 1.0);

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

    bool SceneRenderer::CreateLineBuffer(
        const void* data,
        int count,
        unsigned int& vertexArray,
        unsigned int& vertexBuffer)
    {
        if (!data || count <= 0)
            return false;

        glGenVertexArrays(1, &vertexArray);
        glGenBuffers(1, &vertexBuffer);

        if (!vertexArray || !vertexBuffer)
            return false;

        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(
                static_cast<std::size_t>(count) *
                sizeof(SceneLineVertex)
                ),
            data,
            GL_STATIC_DRAW
        );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneLineVertex),
            reinterpret_cast<void*>(0)
        );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(SceneLineVertex),
            reinterpret_cast<void*>(
                offsetof(SceneLineVertex, color)
                )
        );

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return true;
    }

    bool SceneRenderer::CreateGrid()
    {
        const auto vertices = BuildSceneGridVertices();

        gridVertexCount = static_cast<int>(vertices.size());

        return CreateLineBuffer(
            vertices.data(),
            gridVertexCount,
            gridVertexArray,
            gridVertexBuffer
        );
    }

    bool SceneRenderer::CreateAxis()
    {
        const auto vertices = BuildSceneAxisVertices();

        axisVertexCount = static_cast<int>(vertices.size());

        return CreateLineBuffer(
            vertices.data(),
            axisVertexCount,
            axisVertexArray,
            axisVertexBuffer
        );
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
        int previousTexture = 0;
        int previousRenderbuffer = 0;

        glGetIntegerv(
            GL_FRAMEBUFFER_BINDING,
            &previousFramebuffer
        );

        glGetIntegerv(
            GL_TEXTURE_BINDING_2D,
            &previousTexture
        );

        glGetIntegerv(
            GL_RENDERBUFFER_BINDING,
            &previousRenderbuffer
        );

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        if (colorTexture)
        {
            glDeleteTextures(1, &colorTexture);
            colorTexture = 0;
        }

        if (depthBuffer)
        {
            glDeleteRenderbuffers(1, &depthBuffer);
            depthBuffer = 0;
        }

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

        glBindTexture(
            GL_TEXTURE_2D,
            static_cast<unsigned int>(previousTexture)
        );

        glBindRenderbuffer(
            GL_RENDERBUFFER,
            static_cast<unsigned int>(previousRenderbuffer)
        );

        if (!complete)
        {
            std::cerr
                << "CNode: Scene framebuffer is incomplete.\n";

            framebufferWidth = 0;
            framebufferHeight = 0;
            return false;
        }

        framebufferWidth = width;
        framebufferHeight = height;

        return true;
    }

    void SceneRenderer::Render(
        const SceneCamera& camera,
        int width,
        int height)
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

        const int matrixLocation = glGetUniformLocation(
            shaderProgram,
            "uViewProjection"
        );

        glUniformMatrix4fv(
            matrixLocation,
            1,
            GL_FALSE,
            viewProjection
        );

        glBindVertexArray(gridVertexArray);
        glDrawArrays(GL_LINES, 0, gridVertexCount);

        glBindVertexArray(axisVertexArray);
        glDrawArrays(GL_LINES, 0, axisVertexCount);

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
        if (gridVertexBuffer)
            glDeleteBuffers(1, &gridVertexBuffer);

        if (gridVertexArray)
            glDeleteVertexArrays(1, &gridVertexArray);

        if (axisVertexBuffer)
            glDeleteBuffers(1, &axisVertexBuffer);

        if (axisVertexArray)
            glDeleteVertexArrays(1, &axisVertexArray);

        if (shaderProgram)
            glDeleteProgram(shaderProgram);

        if (colorTexture)
            glDeleteTextures(1, &colorTexture);

        if (depthBuffer)
            glDeleteRenderbuffers(1, &depthBuffer);

        if (framebuffer)
            glDeleteFramebuffers(1, &framebuffer);

        framebuffer = 0;
        colorTexture = 0;
        depthBuffer = 0;
        shaderProgram = 0;

        gridVertexArray = 0;
        gridVertexBuffer = 0;
        gridVertexCount = 0;

        axisVertexArray = 0;
        axisVertexBuffer = 0;
        axisVertexCount = 0;

        framebufferWidth = 0;
        framebufferHeight = 0;

        initialized = false;
    }
}
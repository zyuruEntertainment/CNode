#include "tab/windows/scene_editor/scene_renderer.h"
#include "tab/windows/scene_editor/scene_grid.h"
#include "tab/windows/scene_editor/scene_axis.h"
#include "tab/windows/project_setting/project_setting_data.h"
#include "scene/scene.h"

#define NOMINMAX
#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <iostream>
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
            !CreateCubeShader() ||
            !CreateCube())
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
        unsigned int& vertexBuffer,
        unsigned int usage)
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
            usage
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

    bool SceneRenderer::CreateCubeShader()
    {
        const char* vertexSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPosition;
            uniform mat4 uModel;
            uniform mat4 uViewProjection;
            void main()
            {
                gl_Position = uViewProjection * uModel * vec4(aPosition, 1.0);
            }
        )";

        const char* fragmentSource = R"(
            #version 330 core
            uniform vec3 uColor;
            out vec4 FragColor;
            void main()
            {
                FragColor = vec4(uColor, 1.0);
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

        cubeShaderProgram = glCreateProgram();
        glAttachShader(cubeShaderProgram, vertexShader);
        glAttachShader(cubeShaderProgram, fragmentShader);
        glLinkProgram(cubeShaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        int success = 0;
        glGetProgramiv(cubeShaderProgram, GL_LINK_STATUS, &success);

        if (!success)
        {
            glDeleteProgram(cubeShaderProgram);
            cubeShaderProgram = 0;
            return false;
        }

        return true;
    }

    bool SceneRenderer::CreateCube()
    {
        const float vertices[] = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f
        };

        const unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            0, 1, 5, 5, 4, 0,
            3, 2, 6, 6, 7, 3,
            0, 3, 7, 7, 4, 0,
            1, 2, 6, 6, 5, 1
        };

        cubeIndexCount = 36;

        glGenVertexArrays(1, &cubeVertexArray);
        glGenBuffers(1, &cubeVertexBuffer);
        glGenBuffers(1, &cubeIndexBuffer);

        if (!cubeVertexArray || !cubeVertexBuffer || !cubeIndexBuffer)
            return false;

        glBindVertexArray(cubeVertexArray);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            vertices,
            GL_STATIC_DRAW
        );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIndexBuffer);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(indices),
            indices,
            GL_STATIC_DRAW
        );

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(float),
            reinterpret_cast<void*>(0)
        );

        glBindVertexArray(0);

        return true;
    }

    bool SceneRenderer::CreateGrid(
        const SceneCamera& camera,
        int width,
        int height)
    {
        const auto vertices = BuildSceneGridVertices(
            camera, width, height
        );

        gridVertexCount = static_cast<int>(vertices.minor.size());
        gridMajorVertexCount =
            static_cast<int>(vertices.major.size());

        const bool minorOk = CreateLineBuffer(
            vertices.minor.data(),
            gridVertexCount,
            gridVertexArray,
            gridVertexBuffer,
            0x88E8
        );

        const bool majorOk = CreateLineBuffer(
            vertices.major.data(),
            gridMajorVertexCount,
            gridMajorVertexArray,
            gridMajorVertexBuffer,
            0x88E8
        );

        return minorOk && majorOk;
    }

    bool SceneRenderer::UpdateGrid(
        const SceneCamera& camera,
        int width,
        int height)
    {
        const auto vertices = BuildSceneGridVertices(
            camera, width, height
        );

        gridVertexCount = static_cast<int>(vertices.minor.size());
        gridMajorVertexCount =
            static_cast<int>(vertices.major.size());

        if (gridVertexCount > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, gridVertexBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(
                    vertices.minor.size() *
                    sizeof(SceneLineVertex)
                ),
                vertices.minor.data(),
                0x88E8
            );
        }

        if (gridMajorVertexCount > 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, gridMajorVertexBuffer);
            glBufferData(
                GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(
                    vertices.major.size() *
                    sizeof(SceneLineVertex)
                ),
                vertices.major.data(),
                0x88E8
            );
        }

        return true;
    }

    bool SceneRenderer::CreateAxis(const SceneCamera& camera)
    {
        const auto vertices = BuildSceneAxisVertices(camera);

        axisVertexCount = static_cast<int>(vertices.size());

        return CreateLineBuffer(
            vertices.data(),
            axisVertexCount,
            axisVertexArray,
            axisVertexBuffer,
            0x88E8
        );
    }

    bool SceneRenderer::UpdateAxis(const SceneCamera& camera)
    {
        const auto vertices = BuildSceneAxisVertices(camera);

        if (vertices.empty())
            return false;

        glBindBuffer(GL_ARRAY_BUFFER, axisVertexBuffer);

        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(
                vertices.size() * sizeof(SceneLineVertex)
                ),
            vertices.data(),
            0x88E8
        );

        axisVertexCount = static_cast<int>(vertices.size());
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

        const ProjectSettings& projectSettings =
            GetProjectSettings();

        glClearColor(
            projectSettings.backgroundColor[0],
            projectSettings.backgroundColor[1],
            projectSettings.backgroundColor[2],
            1.0f
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float viewProjection[16] = {};

        camera.GetViewProjection(
            static_cast<float>(width),
            static_cast<float>(height),
            viewProjection
        );

        if (GetProjectSettings().showGrid)
        {
            if (gridVertexArray == 0)
            {
                if (!CreateGrid(camera, width, height))
                    return;
            }
            else
            {
                UpdateGrid(camera, width, height);
            }
        }

        if (GetProjectSettings().showAxis)
        {
            if (axisVertexArray == 0)
            {
                if (!CreateAxis(camera))
                    return;
            }
            else
            {
                UpdateAxis(camera);
            }
        }

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

        if (projectSettings.showGrid)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_LINE_SMOOTH);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

            if (gridVertexCount > 0 && gridVertexArray != 0)
            {
                glLineWidth(1.0f);
                glBindVertexArray(gridVertexArray);
                glDrawArrays(GL_LINES, 0, gridVertexCount);
            }

            if (gridMajorVertexCount > 0 && gridMajorVertexArray != 0)
            {
                glLineWidth(2.0f);
                glBindVertexArray(gridMajorVertexArray);
                glDrawArrays(GL_LINES, 0, gridMajorVertexCount);
            }

            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_BLEND);
        }

        if (projectSettings.showAxis && axisVertexArray != 0)
        {
            glBindVertexArray(axisVertexArray);
            glDrawArrays(GL_LINES, 0, axisVertexCount);
        }

        glUseProgram(cubeShaderProgram);

        const int modelLocation = glGetUniformLocation(
            cubeShaderProgram,
            "uModel"
        );

        const int cubeMatrixLocation = glGetUniformLocation(
            cubeShaderProgram,
            "uViewProjection"
        );

        const int colorLocation = glGetUniformLocation(
            cubeShaderProgram,
            "uColor"
        );

        glUniformMatrix4fv(
            cubeMatrixLocation,
            1,
            GL_FALSE,
            viewProjection
        );

        const Scene& scene = GetScene();

        for (const SceneObject& object : scene.objects)
        {
            if (!object.renderer.enabled)
                continue;

            const float px = object.transform.position.x;
            const float py = object.transform.position.y;
            const float pz = object.transform.position.z;

            const float rx = object.transform.rotation.x * 3.14159265f / 180.0f;
            const float ry = object.transform.rotation.y * 3.14159265f / 180.0f;
            const float rz = object.transform.rotation.z * 3.14159265f / 180.0f;

            const float sx = object.transform.scale.x;
            const float sy = object.transform.scale.y;
            const float sz = object.transform.scale.z;

            float model[16] = {
                1,0,0,0,
                0,1,0,0,
                0,0,1,0,
                0,0,0,1
            };

            float rotX[16], rotY[16], rotZ[16], scale[16], trans[16], temp[16];

            auto identity = [](float* m) {
                for (int i = 0; i < 16; ++i) m[i] = 0.0f;
                m[0] = m[5] = m[10] = m[15] = 1.0f;
            };

            auto multiply = [](const float* a, const float* b, float* out) {
                float r[16] = {};
                for (int c = 0; c < 4; ++c)
                    for (int rrow = 0; rrow < 4; ++rrow)
                        for (int k = 0; k < 4; ++k)
                            r[c * 4 + rrow] += a[k * 4 + rrow] * b[c * 4 + k];
                for (int i = 0; i < 16; ++i) out[i] = r[i];
            };

            identity(rotX);
            rotX[5] = std::cos(rx);
            rotX[6] = std::sin(rx);
            rotX[9] = -std::sin(rx);
            rotX[10] = std::cos(rx);

            identity(rotY);
            rotY[0] = std::cos(ry);
            rotY[2] = -std::sin(ry);
            rotY[8] = std::sin(ry);
            rotY[10] = std::cos(ry);

            identity(rotZ);
            rotZ[0] = std::cos(rz);
            rotZ[1] = std::sin(rz);
            rotZ[4] = -std::sin(rz);
            rotZ[5] = std::cos(rz);

            identity(scale);
            scale[0] = sx;
            scale[5] = sy;
            scale[10] = sz;

            identity(trans);
            trans[12] = px;
            trans[13] = py;
            trans[14] = pz;

            multiply(rotZ, scale, temp);
            multiply(rotY, temp, temp);
            multiply(rotX, temp, temp);
            multiply(trans, temp, model);

            (void)model;

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, model);
            glUniform3f(
                colorLocation,
                object.renderer.color[0],
                object.renderer.color[1],
                object.renderer.color[2]
            );

            glBindVertexArray(cubeVertexArray);
            glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, nullptr);

            continue;
        }

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

        if (gridMajorVertexBuffer)
            glDeleteBuffers(1, &gridMajorVertexBuffer);

        if (gridMajorVertexArray)
            glDeleteVertexArrays(1, &gridMajorVertexArray);

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

        gridMajorVertexArray = 0;
        gridMajorVertexBuffer = 0;
        gridMajorVertexCount = 0;

        axisVertexArray = 0;
        axisVertexBuffer = 0;
        axisVertexCount = 0;

        framebufferWidth = 0;
        framebufferHeight = 0;

        initialized = false;
    }
}
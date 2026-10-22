cmake_minimum_required(VERSION 3.24)

cmake_policy(SET CMP0169 OLD)

project(CNode LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(stb)

if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

if(NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "Dear ImGui not found in external/imgui")
endif()

find_package(OpenGL REQUIRED)

file(
    GLOB_RECURSE CNODE_REGISTRATION_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*_registration.h"
)

list(LENGTH CNODE_REGISTRATION_HEADERS CNODE_REGISTRATION_COUNT)

if(CNODE_REGISTRATION_COUNT EQUAL 0)
    message(FATAL_ERROR
        "CNode: No feature registration headers found in include/"
    )
endif()

set(CNODE_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

file(MAKE_DIRECTORY "${CNODE_GENERATED_DIR}")

set(CNODE_REGISTRATION_SOURCE
    "${CNODE_GENERATED_DIR}/registration_registry.cpp"
)

set(CNODE_REGISTRATION_INCLUDES
    "#include \"registration/registration_registry.h\"\n"
)

set(CNODE_REGISTRATION_CALLS "")

set(CNODE_REGISTRATION_FUNCTIONS "")

foreach(REGISTRATION_HEADER IN LISTS CNODE_REGISTRATION_HEADERS)
    file(
        RELATIVE_PATH REGISTRATION_RELATIVE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${REGISTRATION_HEADER}"
    )

    file(
        TO_CMAKE_PATH
        "${REGISTRATION_RELATIVE_PATH}"
        REGISTRATION_RELATIVE_PATH
    )

    string(
        REGEX REPLACE
        "_registration\\.h$"
        ""
        REGISTRATION_FUNCTION_SUFFIX
        "${REGISTRATION_RELATIVE_PATH}"
    )

    string(
        REGEX REPLACE
        "[^A-Za-z0-9_]"
        "_"
        REGISTRATION_FUNCTION_SUFFIX
        "${REGISTRATION_FUNCTION_SUFFIX}"
    )

    list(FIND CNODE_REGISTRATION_FUNCTIONS
        "${REGISTRATION_FUNCTION_SUFFIX}"
        REGISTRATION_FUNCTION_INDEX
    )

    if(NOT REGISTRATION_FUNCTION_INDEX EQUAL -1)
        message(FATAL_ERROR
            "Duplicate registration function suffix: ${REGISTRATION_FUNCTION_SUFFIX}"
        )
    endif()

    list(APPEND CNODE_REGISTRATION_FUNCTIONS
        "${REGISTRATION_FUNCTION_SUFFIX}"
    )

    string(
        APPEND CNODE_REGISTRATION_INCLUDES
        "#include \"${REGISTRATION_RELATIVE_PATH}\"\n"
    )

    string(
        APPEND CNODE_REGISTRATION_CALLS
        "        RegisterFeature_${REGISTRATION_FUNCTION_SUFFIX}();\n"
    )
endforeach()

string(
    CONCAT CNODE_REGISTRATION_CONTENT
    "${CNODE_REGISTRATION_INCLUDES}\n"
    "namespace CNode\n"
    "{\n"
    "    void RegisterAllFeatures()\n"
    "    {\n"
    "        static const bool registered = []()\n"
    "        {\n"
    "${CNODE_REGISTRATION_CALLS}"
    "            return true;\n"
    "        }();\n"
    "\n"
    "        (void)registered;\n"
    "    }\n"
    "}\n"
)

file(
    CONFIGURE
    OUTPUT "${CNODE_REGISTRATION_SOURCE}"
    CONTENT "${CNODE_REGISTRATION_CONTENT}"
    @ONLY
)

message(STATUS
    "CNode: Found ${CNODE_REGISTRATION_COUNT} feature registration headers"
)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glad)

glad_add_library(glad_gl_core_33
    REPRODUCIBLE
    API gl:core=3.3
)

add_executable(CNode
    main.cpp

    src/scene/window/editor.cpp
    src/scene/window/logo.cpp
    src/scene/window/font.cpp

    src/registration/registration.cpp
    src/registration/registration_data.cpp
    src/registration/window_docking.cpp
    src/registration/default_layout.cpp

    src/tab/windows/inspector/inspector.cpp
    src/tab/windows/node_graph/node_graph.cpp
    src/tab/windows/object_list/object_list.cpp
    src/tab/windows/project_setting/project_setting.cpp

    src/tab/windows/window_id.cpp
    src/tab/windows/window_registry.cpp
    src/tab/windows/window_instance_manager.cpp

    src/tab/windows/scene_editor/scene_editor.cpp
    src/tab/windows/scene_editor/scene_camera.cpp
    src/tab/windows/scene_editor/scene_grid.cpp
    src/tab/windows/scene_editor/scene_axis.cpp
    src/tab/windows/scene_editor/scene_renderer.cpp
    src/tab/windows/scene_editor/navigation/orbit_navigation.cpp
    src/tab/windows/scene_editor/navigation/zoom_navigation.cpp
    src/tab/windows/scene_editor/navigation/pan_navigation.cpp
    src/tab/windows/scene_editor/navigation/free_look_navigation.cpp
    src/tab/windows/scene_editor/ui/orbit_gizmo.cpp
    src/tab/windows/scene_editor/ui/pan_view_controller.cpp


    src/core/input/cursor_visibility.cpp

    "${CNODE_REGISTRATION_SOURCE}"

    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"

    "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
 "src/tab/windows/scene_editor/navigation/pan_navigation.cpp" "include/tab/windows/scene_editor/navigation/free_look_navigation.h" "src/tab/windows/scene_editor/navigation/free_look_navigation.cpp")

target_compile_definitions(CNode PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(CNode PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends"
    "${stb_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/gladsources/glad_gl_core_33/include"
)

target_link_libraries(CNode PRIVATE
    glfw
    OpenGL::GL
    glad_gl_core_33
)

set_property(
    TARGET CNode
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
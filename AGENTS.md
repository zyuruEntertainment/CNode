# Build & Run (C++20, CMake)
- **Configure/build**: `cmake -S . -B build` then build with your generator (e.g. `cmake --build build --config Debug` or open `build/*.sln` in Visual Studio). Debug exe is at `build/Debug/CNode.exe`.
- **Dependencies**: `glfw` fetched via CMake FetchContent (3.4). `stb` fetched from GitHub. `glad` and `imgui` must exist under `external/glad` and `external/imgui` (CMake errors if missing). `external/` is vendored.
- **Working dir**: Debugger working directory is repo root (`${CMAKE_CURRENT_SOURCE_DIR}`) - assets/state are loaded relative to repo root.
- **Entry**: `main.cpp` -> `CNode::EditorWindow`; calls `CNode::Windows()` then `editor.Run()`/`Shutdown()`.

# Codegen / Registration
- **Auto-registration**: CMake globs `include/**/*_registration.h` and generates `${CMAKE_BINARY_DIR}/generated/registration_registry.cpp` with `CNode::RegisterAllFeatures()`. Build fails if no registration headers found or duplicate function suffixes.
- **Adding a window/feature**: create `include/<path>/*_registration.h` declaring `inline void RegisterFeature_<sanitized_path>()` that calls `RegisterWindowAttribute(...)` (see `include/tab/windows/node_graph/node_graph_registration.h`). The function name is derived from the header path with non-alphanumerics replaced by underscores.
- **Registration flow**: `RegisterAllFeatures()` is generated and linked; `CNode::Windows()` (in `src/registration/registration_data.cpp`) calls it. `DrawRegisteredWindows()` in `src/registration/registration.cpp` renders window menu/dockspace and instantiates windows.

# Architecture
- **Modular windows/tabs**: Windows registered via `tab/windows/window_registry` and instantiated by `window_instance_manager`. Implementations in `src/tab/windows/...` with headers in `include/tab/windows/...`.
- **Editor core**: `src/scene/window/{editor,logo,font}.cpp`, docking via `src/registration/window_docking.cpp`, default layout persisted as `.cnode_default_layout_v1`.
- **Scene editor**: Has navigation/camera/rendering split under `src/tab/windows/scene_editor/{navigation,ui}/`.

# Gotchas
- **No tests**: No test targets/scripts found; treat as manual verification in the running app.
- **Generated file**: `build/generated/registration_registry.cpp` is generated at configure time - don't edit it.
- **Vendored deps in external/**: `external/imgui` and `external/glad` must remain present (build checks for specific files).
- **C++20** required. Use existing patterns (e.g. inline registration functions).  

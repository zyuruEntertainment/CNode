# CNode Engine (C++20, CMake)

## Build & Run
- Configure: `cmake -S . -B build` (CMake >=3.24; first configure needs network for GLFW/stb FetchContent).
- Build: `cmake --build build --config Debug` (multi-config/Visual Studio) or `cmake --build build` (single-config). VS solution is `build/CNode.sln` when using the Visual Studio generator.
- Run from repo root: assets and runtime state (`.cnode_default_layout_v1`, `imgui.ini`) resolve relative to CWD. The VS debugger working dir is already repo root; launching `build/Debug/CNode.exe` directly without changing CWD will fail to load assets.
- Debug binary: `build/Debug/CNode.exe` (multi-config) or `build/CNode.exe` (single-config).
- Vendored deps `external/imgui` and `external/glad` must exist (CMake errors otherwise); `glfw` and `stb` are fetched.

## Codegen / Registration
- CMake globs `include/**/*_registration.h` and generates `build/generated/registration_registry.cpp` with `CNode::RegisterAllFeatures()`. Build fails if no headers are found or sanitized suffixes collide.
- Add a window/feature:
  1. Add implementation under `src/...` and header under `include/...` with mirrored paths.
  2. Add the new `.cpp` to the explicit source list in `add_executable(CNode ...)` in `CMakeLists.txt` (src is not globbed).
  3. Add `include/<path>/*_registration.h` declaring `inline void RegisterFeature_<sanitized_path>()` that calls `RegisterWindowAttribute(...)` (pattern: `include/tab/windows/node_graph/node_graph_registration.h`). Sanitized path = relative include path with `_registration.h` removed and non-alphanumerics replaced by `_`.
  4. Reconfigure if needed (`cmake -S . -B build`), then build.
- Flow: `main.cpp` -> `CNode::EditorWindow`; `CNode::Windows()` (`src/registration/registration_data.cpp`) calls `RegisterAllFeatures()` then `RegisterWindowAttributes()`; `DrawRegisteredWindows()` (`src/registration/registration.cpp`) renders the Windows menu/dockspace and draws instances. Feature headers must call `RegisterWindowAttribute`, not `RegisterWindowType`.

## Architecture
- Windows/tabs: registered via `tab/windows/window_registry`, instantiated by `window_instance_manager`; impl in `src/tab/windows/...`, headers in `include/tab/windows/...`.
- Editor core: `src/scene/window/{editor,logo,font}.cpp`; docking in `src/registration/window_docking.cpp`; default layout marker `.cnode_default_layout_v1`.
- Scene editor: camera/grid/axis/renderer plus `navigation/` and `ui/` under `src/tab/windows/scene_editor/`.
- Default dock layout builds once per marker version (default windows: scene_editor, object_list, node_graph, inspector). Delete `.cnode_default_layout_v1` (and `imgui.ini` for a full UI reset) to rebuild.

## Gotchas
- No tests: verify manually by building and running the app.
- Do not edit `build/generated/registration_registry.cpp`.
- C++20 required (`CMAKE_CXX_STANDARD 20`).
- Closing a scene editor window destroys its state (`DestroySceneEditorState`); reopening recreates it.
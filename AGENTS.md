# CNode Engine — Agent Notes

CNode is a C++20 node-based game engine (GLFW + Dear ImGui + OpenGL). Single executable target: `CNode`.

## Build

- Configure + build:
  ```
  cmake -S . -B build
  cmake --build build
  ```
  `build/` already exists with a configured Makefile; `cmake --build build` is enough for iteration.
- CMake 3.24+, C++20 required (standard required, extensions off).
- First configure needs internet: `FetchContent` pulls GLFW (github.com/glfw/glfw, tag 3.4) and stb (nothings/stb, master). `glad` and `imgui` are vendored under `external/`; CMake fatals if `external/imgui/imgui.cpp` or `external/glad/include/...`/`src/glad.c` are missing.
- `find_package(OpenGL REQUIRED)` — system OpenGL must be installed.
- No `CMakePresets.json`, no CI, no test suite, no lint/formatter config. Match surrounding file style when editing.

## Code generation (easy to miss)

- CMake scans `include/**/*_registration.h` at **configure time** and writes `build/generated/registration_registry.cpp`, implementing `CNode::RegisterAllFeatures()` which calls each `RegisterFeature_<sanitized_path>()`.
- To add a feature: create `include/<path>/<name>_registration.h` defining `inline void RegisterFeature_<...>()`, then re-run `cmake -S . -B build` so the registry regenerates before building. Duplicate registration suffixes are a configure-time fatal error.
- `src/registration/registration.cpp` is hand-maintained; the generated registry file is compiled into the target.

## Layout

- `src/` + `include/` mirror: `components`, `core`, `nodes`, `registration`, `scene`, `tab`, `ui`.
- Entry point: `main.cpp` → `CNode::EditorWindow` (`Initialize` → `Windows` → `Run` → `Shutdown`).
- `external/` — vendored glad + imgui (do not edit).
- `build/`, `out/`, `.vs/` — gitignored outputs.
- `notes/`, `fix*.py` — scratch/dev notes and one-off fixes; not documentation.
- Runtime state `imgui.ini` and `.cnode_default_layout_v1` are written next to the executable/source root and gitignored.

## Default dock layout

- `src/registration/default_layout.cpp` builds the initial dock layout on first launch.
- `include/registration/default_layout.h` declares `SetupDefaultDockLayout()`.
- The layout is applied only once: a marker file `.cnode_default_layout_v1` is written after the first apply. On later launches Dear ImGui restores the user's last layout from `imgui.ini` (auto-saved by `io.IniFilename = "imgui.ini"`).
- To reset to the default layout, delete `.cnode_default_layout_v1` (and `imgui.ini` if you want a fully clean slate).
- Current default: Left column (~60% width): Scene Editor (top ~65%) above Node Graph (bottom ~35%). Right column (~40% width): Object List (left ~50%) beside Inspector (right ~50%), separate panels — not tabbed.

## Verification

- No tests exist. Verify by building and running `build/CNode.exe` (VS debugger working directory is the source root).

## Grid rendering (easy to regress)

- `src/tab/windows/scene_editor/scene_grid.cpp` builds the XZ world-space grid.
- Both X-direction and Z-direction line families must always be rendered; do **not** scale either family's opacity by camera direction (e.g. `abs(viewDirX)` / `abs(viewDirZ)`). That caused one family to disappear when looking along an axis.
- **Adaptive screen-space LOD**: a `ScreenFade()` multiplier fades minor grid lines whose projected screen-space spacing falls below ~2–5 px, and major lines below ~1–3 px. This prevents moiré and dense line merging near the horizon without camera-direction conditions.
- Depth per line is estimated via dot product with the normalized camera view direction; screen spacing = `spacing * viewportScale / (tan(FOV/2) * depth)`.
- Fade is combined multiplicatively: `combinedFade = SmoothFade(worldDistance, radius) * ScreenFade(screenSpacing)`.
- Major lines use lower pixel thresholds so they remain visible longer than minor lines.
- Grid lines are world-space aligned; camera movement/rotation only changes projection via `uViewProjection`.
- Rendered with `GL_LINE_SMOOTH` + `GL_SRC_ALPHA`/`GL_ONE_MINUS_SRC_ALPHA` blending (1.0 px minor, 2.0 px major).

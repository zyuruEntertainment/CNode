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

- `src/tab/windows/scene_editor/scene_renderer.cpp` renders the grid with a procedural fragment shader (`CreateGridShader()`), replacing the old CPU line-generation approach (`scene_grid.cpp` is now a stub).
- The shader draws a full-screen quad and computes a ray-plane intersection with the XZ ground plane (Y=0) per fragment. No depth texture is needed — the world position is derived analytically from the camera basis and NDC.
- Grid lines are computed in world space using `fract(x/spacing)` and `fract(z/spacing)`, with `fwidth()`-based anti-aliasing for stable line coverage at grazing angles.
- **Adaptive LOD**: spacing is distance-based power-of-two (`spacing = pow(2, floor(log2(dist) * 0.5))`), clamped to [0.25, 256]. This keeps visual line density stable and prevents dense horizon bands without camera-direction conditions.
- Major lines every 10th subdivision are brighter; they remain visible longer than minor lines.
- Distance fading: `smoothstep(renderDistance * 0.5, renderDistance, dist)`, inverted so near = clear, far = background.
- Horizon/grazing fade: `smoothstep(0.0, 0.2, abs(rayDir.y))` prevents compressed lines near the horizon.
- Line edge width: `fwidth(grid) * 0.5 + 0.01` for thin crisp minor lines; same formula for major lines.
- Grid combine: `max(minorGrid, majorGrid)` (no `* 1.6` multiplier — avoids glowing bands). Major lines use a single mix with brighter color `{0.30, 0.33, 0.40}` — no double blending.
- Both X and Z grid families are always rendered — camera azimuth never disables either family. This is a regression guard from the earlier camera-direction-scaling bug.
- Grid is drawn first with depth write disabled, so axes and cubes occlude it correctly.
- Rendered with `GL_SRC_ALPHA`/`GL_ONE_MINUS_SRC_ALPHA` blending (no `GL_LINE_SMOOTH` — the shader handles anti-aliasing analytically).
- Grid color: minor {0.18, 0.20, 0.25}, major {0.30, 0.33, 0.40}, blended against background {0.075, 0.085, 0.10}.

## Camera navigation (regression guard)

- The grid shader computes camera basis vectors (`right`, `up`, `forward`) locally for ray construction. The right vector **must** be computed as `forward × up` (cross(forward, up)), matching `pan_navigation.cpp` and the view matrix `LookAt()`. Using `up × forward` mirrors the grid horizontally and makes navigation feel inverted.
- The grid shader's basis computation does **not** modify the camera object — navigation code in `navigation/` is completely separate.

# Architecture decisions (initial)

## ADR-001 — C++20 and CMake

**Decision:** C++20 for engine/runtime code, CMake 3.20 as the project build entry point, CTest for executable tests.

**Reason:** C++ gives the project direct control over desktop runtime performance and native integration; CMake provides configure/build/test support across the intended desktop toolchains. The root build keeps the language library testable independently of graphics.

**Status:** Implemented for the initial targets. Compiler/platform coverage is only confirmed when CI results are available.

## ADR-002 — GLFW 3.5.1 plus OpenGL 3.3 Core for the first renderer

**Decision:** GLFW 3.5.1 for desktop windows, context creation, and input; OpenGL 3.3 Core with shaders and a minimal function loader for the first graphics path.

**Alternatives considered:** SDL's window/input facilities and Vulkan. GLFW's official docs directly demonstrate the narrow window/context/input loop needed for the first vertical slice. OpenGL 3.3 Core makes a basic cross-platform renderer smaller than a Vulkan bootstrap while avoiding deprecated fixed-function drawing. Vulkan remains a possible later path if measured requirements justify its more extensive setup.

**Trade-offs:** GLFW is an external dependency pinned by release tag and SHA-256 archive hash and fetched by CMake; configure therefore needs network access unless a local compatible package is supplied. CMake FetchContent documents URL_HASH for verifying downloaded archives. OpenGL 3.3 support and desktop display availability still need platform execution testing. The initial city mesh is deliberately procedural and temporary.

**Licensing:** GLFW is distributed under the zlib/libpng license. No GLFW source is copied into this repository.

## ADR-003 — PatrolScript begins as a standalone language runtime

**Decision:** Build a C++ lexer, recursive-descent parser, AST, and interpreter under `language/`. Keep engine APIs behind future host bindings instead of allowing scripts direct native access.

**Reason:** Parser/runtime tests can run in headless CI without a graphics context. That isolates syntax and semantic defects from graphics/platform failures.

**Current limitation:** Only core expressions, lexical blocks, `let`, `fn`, `if`, `while`, `return`, and `print` plus `clock()`/`abs()` exist. Mission callbacks and game API bindings are not implemented.

## ADR-004 — glTF 2.0/GLB for interchange; Blender source remains authored source

**Decision:** Use Blender for original models and document glTF 2.0/GLB as the target interchange format. Keep `.blend` source when generated art is added and storage allows it. Model import, validation, collision meshes, and asset caching are separate follow-on work, not implied by this decision.

**Reason:** glTF 2.0 is a standardized runtime asset format for geometry, materials, textures, cameras, lights, and animation. This stage draws its blockout procedurally; no GLB file is loaded by the engine yet.

## Initial module boundaries

- `police_core`: logger and later engine lifecycle/time/IDs/errors.
- `police_platform`: GLFW window/context/input facade.
- `police_renderer`: OpenGL loader, shader setup, camera and procedural blockout mesh.
- `patrolscript`: scanner/parser/AST/interpreter with no graphics dependency.
- `POLICE-Stimulator`: current application loop, composed from platform and renderer.

Physics, audio, animation, AI, dispatch, gameplay state, imported assets, and editor/debug UI remain future modules until concrete code exists for them.

# Architecture decisions

## ADR-001 — C++20 and CMake

Decision: use C++20 for engine/runtime code, CMake 3.20+ as the build entry point and CTest for test execution. The language runtime and vehicle dynamics are separate libraries so they can be tested without a graphics context.

## ADR-002 — project-owned native platform layer

Decision: avoid a third-party windowing/input library. The police_platform module implements native Win32/WGL, X11/GLX, and Cocoa/NSOpenGL backends behind a small project-owned Window interface. The renderer uses a project-owned OpenGL function loader.

Trade-off: native backends cost more maintenance and have OS-specific failure modes. The current Linux backend uses X11/GLX (and can run through XWayland), not the pure Wayland protocol. macOS exposes a 3.2 Core context; shader compatibility must be checked on actual Apple hardware. The engine does not implement graphics drivers; it uses the OS/driver API as the hardware boundary.

## ADR-003 — OpenGL first renderer

Decision: OpenGL Core is the first 3D path because it keeps an initial cross-platform renderer smaller than a Vulkan bootstrap. Procedural meshes are temporary blockout content. Shader, mesh, camera and render-loop code live in this repository.

## ADR-004 — PatrolScript standalone language

Decision: the C++ lexer, recursive-descent parser, AST and interpreter live under language/. Script code receives only explicit host APIs; no direct native access is exposed. Current built-ins are intentionally limited to clock() and abs(), and mission/dispatch APIs are not yet connected.

## ADR-005 — from-scratch vehicle prototype

Decision: keep basic acceleration, reverse, braking, steering integration, emergency-light/siren state and unit tests in game/vehicles/. This is a controllable kinematic/arcade prototype, not a tire/friction model or collision system.

## ADR-006 — Blender and GLB asset interchange

Decision: use Blender for original authored 3D content and GLB/glTF as interchange. The runtime GLB importer, asset validation, collision-mesh loading and cache are future work and must have tests before being advertised as functional.

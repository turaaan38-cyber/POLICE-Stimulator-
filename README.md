# POLICE-Stimulator

A from-scratch, original 3D police-simulation codebase. The project owns its engine modules, PatrolScript compiler/interpreter, vehicle movement, procedural blockout, test suite, and Blender authoring tools. It contains no third-party game engine or proprietary game assets.

## Native foundation (0.3 development)

- C++20 engine and CMake/CTest build.
- Native window/input/context backends written in this repository: Win32/WGL on Windows, X11/GLX on Linux (including XWayland sessions), and Cocoa/NSOpenGL on macOS. GLFW has been removed from the build. Operating-system and graphics-driver APIs remain platform interfaces, not external engine dependencies.
- In-project OpenGL function loader and OpenGL Core procedural blockout renderer.
- Original low-poly placeholder city and a moving patrol vehicle.
- In-project arcade-style vehicle controller: throttle, service brake/reverse, steering, rolling drag, handbrake, emergency-light state and siren state. This is a deterministic prototype, not tire physics or collision simulation.
- PatrolScript 0.1 lexer, recursive-descent parser, AST, interpreter, lexical scopes, functions, conditionals, loops, CLI, diagnostics and unit tests.
- Blender Python tool for generating an original placeholder patrol car and exporting GLB plus .blend source.

## Build and test

Requirements: CMake 3.20+, C++20 compiler, and system development headers for the native backend. Linux needs X11 and OpenGL development files (libx11-dev, libgl1-mesa-dev on Ubuntu/Debian). Windows uses Win32/WGL SDK components. macOS uses Cocoa and OpenGL system frameworks.

~~~sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
~~~

Run the interactive blockout:

~~~sh
./build/POLICE-Stimulator
~~~

Controls: W throttle, S brake/reverse, A/D steer, Space handbrake, L toggle emergency lights, H toggle the siren state, arrow keys orbit the camera, Escape exit. The siren currently changes state only; audio playback has not been implemented.

Run PatrolScript:

~~~sh
./build/patrolc language/examples/first_patrol.patrol
./build/patrolc --check language/examples/first_patrol.patrol
~~~

For headless language and vehicle unit tests without graphics dependencies:

~~~sh
cmake -S . -B build-headless -DPOLICE_BUILD_VIEWER=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-headless --parallel 2
ctest --test-dir build-headless -C Release --output-on-failure
~~~

## Architecture and asset pipeline

- Architecture decisions: docs/architecture/decisions.md
- PatrolScript specification: docs/language/specification.md
- Build and CI notes: docs/development/build-and-test.md
- Blender/glTF workflow: docs/asset-pipeline/blender-gltf.md

## Current limitations (not yet implemented)

This is an early blockout, not a completed simulator. The renderer does not yet load GLB files; no imported Blender model is drawn by runtime. Collision shapes, rigid-body vehicle dynamics, audio output, character movement/animation, traffic and pedestrian AI, dispatch/callouts, police interactions, mission progression, save data, and full UI are not implemented. The vehicle is controllable but does not collide with buildings or other vehicles. Linux native support currently targets X11/GLX; pure Wayland support is not yet implemented.

The platform layer and core game systems are original project code. Cocoa, Win32, X11/GLX and OpenGL are used as system APIs/drivers, rather than bundling a third-party windowing or game-engine library.

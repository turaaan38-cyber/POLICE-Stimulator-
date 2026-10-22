# POLICE-Stimulator

An original, modular 3D police-simulation project. The repository owns its engine code, PatrolScript language, game logic, tests, and asset tooling. The project takes broad gameplay inspiration from police-patrol simulators; it does not include third-party game code, maps, models, branding, or other proprietary assets.

## Implemented in the current foundation

- A CMake/C++20 build with a small Core logger and separate Platform and Renderer targets.
- An interactive OpenGL 3.3 Core viewer using GLFW 3.5.1. It renders an **original procedural blockout**: roads, simplified buildings, windows, and a low-poly placeholder patrol car. Use **W/A/S/D** to move, arrow keys to look, **Shift** to move faster, and **Escape** to exit. The viewer is a blockout, not a finished game or final art.
- PatrolScript 0.1: a custom lexer, recursive-descent parser, AST, interpreter, lexical scopes, variables, numbers/strings/booleans/nil, arithmetic, comparisons, logic, conditionals, while loops, functions, return values, and basic runtime diagnostics. The current host API is deliberately small (`clock()` and `abs()`); game/dispatch APIs are not connected yet.
- CTest tests and GitHub Actions build/test jobs for Linux, Windows, and macOS.

## Build and run

Requirements: CMake 3.20+, a C++20 compiler, Git, and a desktop OpenGL driver. The default configuration fetches the GLFW 3.5.1 source at configure time and verifies its SHA-256 archive hash. On Linux, install an OpenGL development package and X11 development dependencies, for example `libgl1-mesa-dev` and `xorg-dev` on Ubuntu/Debian.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

Start the 3D blockout viewer:

```sh
./build/POLICE-Stimulator
```

On a multi-configuration generator such as Visual Studio, the executable may be under `build/Release/`.

Run PatrolScript:

```sh
./build/patrolc language/examples/first_patrol.patrol
./build/patrolc --check language/examples/first_patrol.patrol
./build/patrolc --version
```

To build only the language/compiler and its tests without fetching GLFW or requiring OpenGL development files:

```sh
cmake -S . -B build-language-only -DPOLICE_BUILD_VIEWER=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-language-only --parallel 2
ctest --test-dir build-language-only --output-on-failure
```

## Current limitations

The 3D viewer is the first render path only. It is not yet a full engine: it has no imported Blender model runtime, collision/physics system, audio, animation, save data, traffic or pedestrian AI, police gameplay, dispatch, or full UI. The placeholder patrol vehicle is scenery, not drivable. OpenGL rendering currently uses a single procedural vertex mesh and a minimal in-project OpenGL function loader. The renderer will need resource/asset abstractions and performance instrumentation before content scales up.

PatrolScript is an early language implementation and does not yet provide mission declarations such as `mission`/`on_start`, modules, arrays, objects, native engine APIs, or sandbox quotas for memory/recursion. Infinite/very long `while` loops have a one-million-iteration guard; this is not yet a complete sandbox. The executable reports errors with script line and column.

## Architecture and asset pipeline

- [Architecture decisions](docs/architecture/decisions.md)
- [PatrolScript 0.1 specification](docs/language/specification.md)
- [Development / CI](docs/development/build-and-test.md)
- [Blender and glTF/GLB workflow](docs/asset-pipeline/blender-gltf.md)

GLFW is built from its upstream release and remains an external build dependency, not a vendored engine and not a separate project repository. Its upstream source and zlib/libpng license: <https://github.com/glfw/glfw>. glTF 2.0/GLB is the proposed asset interchange format; model import support has not been implemented yet.

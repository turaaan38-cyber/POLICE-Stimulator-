# Build, test, and CI

## Local build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
```

The default configuration fetches GLFW 3.5.1 from the upstream `glfw/glfw` Git repository. Linux build agents need OpenGL and window-system development headers; Ubuntu/Debian CI installs `libgl1-mesa-dev` and `xorg-dev`.

For headless language testing without graphics dependencies:

```sh
cmake -S . -B build-language-only -DPOLICE_BUILD_VIEWER=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-language-only --parallel 2
ctest --test-dir build-language-only --output-on-failure
```

## Continuous integration

`.github/workflows/build-test.yml` configures and builds on Ubuntu, Windows, and macOS, then invokes CTest. CTest covers lexer/parser/runtime cases and checks/runs the sample `.patrol` file. CI compilation does **not** by itself prove a live display, OpenGL driver, input loop, or visual output works; that still needs an interactive machine or a display-backed graphics test.

Report CI as successful only after inspecting the actual workflow run and its jobs/logs. A pushed workflow file or a created run is not proof of a passing run.

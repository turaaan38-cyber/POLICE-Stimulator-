# Build, test, and continuous integration

## Full native desktop build

~~~sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 2
ctest --test-dir build -C Release --output-on-failure
~~~

The window/input/context source is selected by CMake: Win32/WGL on Windows, Cocoa/NSOpenGL on macOS, and X11/GLX on Linux. Linux runners need X11 and OpenGL development headers (libx11-dev, libgl1-mesa-dev on Ubuntu/Debian). No GLFW/SDL windowing package is fetched or linked.

## Headless unit test build

~~~sh
cmake -S . -B build-headless -DPOLICE_BUILD_VIEWER=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build-headless --parallel 2
ctest --test-dir build-headless -C Release --output-on-failure
~~~

## CI

The GitHub Actions matrix configures, builds and runs CTest on Ubuntu, Windows and macOS. This checks compile/test compatibility, but does not prove the viewer opens a desktop window or that rendering looks correct on physical graphics hardware. CI success must be checked against the actual run and job conclusions.

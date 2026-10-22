#include "police/core/Log.hpp"
#include "police/platform/Window.hpp"
#include "police/renderer/SceneRenderer.hpp"
#include <GLFW/glfw3.h>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
int main(int argc, char** argv) {
    if (argc > 1) {
        const std::string argument = argv[1];
        if (argument == "--help" || argument == "-h") {
            std::cout << "POLICE-Stimulator blockout viewer\n"
                         "Controls: W/A/S/D move, arrow keys look, Shift sprint, Escape quit.\n"
                         "This is an early procedural blockout, not the final game.\n";
            return 0;
        }
        if (argument == "--version") { std::cout << "POLICE-Stimulator 0.1.0-dev\n"; return 0; }
        std::cerr << "Unknown argument: " << argument << " (use --help)\n";
        return 2;
    }
    try {
        police::core::log(police::core::LogLevel::Info, "Starting POLICE-Stimulator 0.1.0-dev");
        police::platform::Window window(1280, 720, "POLICE-Stimulator | blockout city");
        police::renderer::SceneRenderer renderer;
        double previous_time = window.time_seconds(), title_elapsed = 0.0;
        unsigned frames = 0;
        while (!window.should_close()) {
            window.poll_events();
            if (window.key_down(GLFW_KEY_ESCAPE)) window.request_close();
            const double now = window.time_seconds(), delta = now - previous_time;
            previous_time = now;
            renderer.update(window, delta);
            renderer.draw(window);
            window.swap_buffers();
            ++frames;
            title_elapsed += delta;
            if (title_elapsed >= 1.0) {
                const double fps = static_cast<double>(frames) / title_elapsed;
                std::ostringstream title;
                title << "POLICE-Stimulator | blockout | " << std::fixed << std::setprecision(0) << fps
                      << " FPS | WASD move, arrows look, Shift sprint, Esc quit";
                window.set_title(title.str()); title_elapsed = 0.0; frames = 0;
            }
        }
        police::core::log(police::core::LogLevel::Info, "Viewer closed cleanly");
    } catch (const std::exception& error) {
        police::core::log(police::core::LogLevel::Error, error.what());
        return 1;
    }
    return 0;
}

#include "police/platform/Window.hpp"
#include "police/core/Log.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>
namespace police::platform {
namespace {
void error_callback(const int code, const char* description) {
    police::core::log(police::core::LogLevel::Error,
        "GLFW " + std::to_string(code) + ": " + (description != nullptr ? description : "unknown error"));
}
}
Window::Window(const int width, const int height, const char* title) {
    glfwSetErrorCallback(error_callback);
    if (glfwInit() != GLFW_TRUE) throw std::runtime_error("GLFW initialization failed; check the display and graphics drivers");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window_ == nullptr) {
        glfwTerminate();
        throw std::runtime_error("could not create an OpenGL window");
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);
}
Window::~Window() {
    if (window_ != nullptr) glfwDestroyWindow(window_);
    glfwTerminate();
}
bool Window::should_close() const { return glfwWindowShouldClose(window_) == GLFW_TRUE; }
bool Window::key_down(const int key) const { return glfwGetKey(window_, key) == GLFW_PRESS; }
void Window::poll_events() const { glfwPollEvents(); }
void Window::swap_buffers() const { glfwSwapBuffers(window_); }
void Window::request_close() const { glfwSetWindowShouldClose(window_, GLFW_TRUE); }
int Window::framebuffer_width() const { int width = 0, height = 0; glfwGetFramebufferSize(window_, &width, &height); return width; }
int Window::framebuffer_height() const { int width = 0, height = 0; glfwGetFramebufferSize(window_, &width, &height); return height; }
double Window::time_seconds() const { return glfwGetTime(); }
void Window::set_title(const std::string& title) const { glfwSetWindowTitle(window_, title.c_str()); }
} // namespace police::platform

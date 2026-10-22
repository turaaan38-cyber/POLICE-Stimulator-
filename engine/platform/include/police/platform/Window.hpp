#pragma once
#include <string>
struct GLFWwindow;
namespace police::platform {
class Window final {
public:
    Window(int width, int height, const char* title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    [[nodiscard]] bool should_close() const;
    [[nodiscard]] bool key_down(int key) const;
    void poll_events() const;
    void swap_buffers() const;
    void request_close() const;
    [[nodiscard]] int framebuffer_width() const;
    [[nodiscard]] int framebuffer_height() const;
    [[nodiscard]] double time_seconds() const;
    void set_title(const std::string& title) const;
    [[nodiscard]] GLFWwindow* native_handle() const noexcept { return window_; }
private:
    GLFWwindow* window_{nullptr};
};
} // namespace police::platform

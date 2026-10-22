#pragma once
#include <cstdint>
#include <memory>
#include <string>
namespace police::platform {
enum class Key : std::uint8_t { W,A,S,D,Space,Escape,Shift,L,H,Left,Right,Up,Down,Count };
class Window final {
public:
    Window(int width,int height,const char* title);
    ~Window();
    Window(const Window&)=delete; Window& operator=(const Window&)=delete;
    Window(Window&&)=delete; Window& operator=(Window&&)=delete;
    [[nodiscard]] bool should_close() const;
    [[nodiscard]] bool key_down(Key key) const;
    void poll_events();
    void swap_buffers() const;
    void request_close();
    [[nodiscard]] int framebuffer_width() const;
    [[nodiscard]] int framebuffer_height() const;
    [[nodiscard]] double time_seconds() const;
    void set_title(const std::string& title);
    [[nodiscard]] void* get_proc_address(const char* name) const;
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
} // namespace police::platform

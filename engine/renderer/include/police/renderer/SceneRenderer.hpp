#pragma once
#include "police/platform/Window.hpp"
#include "police/renderer/GLApi.hpp"
namespace police::renderer {
class SceneRenderer final {
public:
    SceneRenderer();
    ~SceneRenderer();
    SceneRenderer(const SceneRenderer&) = delete;
    SceneRenderer& operator=(const SceneRenderer&) = delete;
    void update(const police::platform::Window& window, double delta_seconds);
    void draw(const police::platform::Window& window) const;
    [[nodiscard]] double camera_x() const noexcept { return camera_x_; }
    [[nodiscard]] double camera_z() const noexcept { return camera_z_; }
private:
    double camera_x_{0.0}, camera_y_{1.7}, camera_z_{14.0};
    double yaw_degrees_{0.0}, pitch_degrees_{0.0};
    GLApi gl_{};
    GLuint program_{0}, vao_{0}, vbo_{0};
    GLint uniform_mvp_{-1};
    GLsizei vertex_count_{0};
};
} // namespace police::renderer

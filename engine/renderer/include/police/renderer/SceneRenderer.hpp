#pragma once
#include "police/platform/Window.hpp"
#include "police/renderer/GLApi.hpp"
#include "police/vehicles/VehicleController.hpp"
namespace police::renderer {
class SceneRenderer final {
public:
 SceneRenderer(); ~SceneRenderer();
 SceneRenderer(const SceneRenderer&)=delete; SceneRenderer& operator=(const SceneRenderer&)=delete;
 void update(const police::platform::Window& window,double delta_seconds);
 void draw(const police::platform::Window& window) const;
 [[nodiscard]] const police::vehicles::VehicleState& vehicle_state() const noexcept { return vehicle_.state(); }
private:
 police::vehicles::PatrolVehicleController vehicle_{};
 double camera_orbit_degrees_{0.0}; double pitch_degrees_{18.0};
 GLApi gl_{};
 GLuint program_{0},city_vao_{0},city_vbo_{0},vehicle_vao_{0},vehicle_vbo_{0};
 GLint uniform_mvp_{-1},uniform_light_phase_{-1},uniform_lights_enabled_{-1};
 GLsizei city_vertex_count_{0},vehicle_vertex_count_{0};
};
} // namespace police::renderer

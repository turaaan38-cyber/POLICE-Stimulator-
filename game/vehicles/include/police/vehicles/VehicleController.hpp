#pragma once
namespace police::vehicles {
struct VehicleInput final {
 bool throttle{false}; bool brake_or_reverse{false}; bool steer_left{false}; bool steer_right{false};
 bool handbrake{false}; bool emergency_lights_pressed{false}; bool siren_pressed{false};
};
struct VehicleState final {
 double x{2.1}; double z{-3.0}; double heading_degrees{0.0}; double speed_mps{0.0};
 bool emergency_lights{false}; bool siren_enabled{false};
};
class PatrolVehicleController final {
public:
 [[nodiscard]] const VehicleState& state() const noexcept { return state_; }
 void update(const VehicleInput& input, double delta_seconds);
private:
 VehicleState state_{}; bool previous_lights_key_{false}; bool previous_siren_key_{false};
};
} // namespace police::vehicles

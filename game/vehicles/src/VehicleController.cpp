#include "police/vehicles/VehicleController.hpp"
#include "police/physics/PhysicsWorld.hpp"
#include <algorithm>
#include <cmath>
namespace police::vehicles { namespace {
constexpr double max_forward_speed=38.0, max_reverse_speed=8.0, acceleration=7.5;
constexpr double reverse_acceleration=3.2, service_braking=13.0, rolling_drag=1.25;
constexpr double handbrake_drag=19.0, steering_gain=0.075, degrees_per_radian=57.29577951308232;
double approach_zero(const double v,const double amount) {
 if(v>0.0)return std::max(0.0,v-amount);
 if(v<0.0)return std::min(0.0,v+amount);
 return 0.0;
}
}
void PatrolVehicleController::update(const VehicleInput& input,const double delta_seconds,const police::physics::PhysicsWorld* collision_world) {
 const double dt=std::clamp(delta_seconds,0.0,0.05);
 if(input.emergency_lights_pressed&&!previous_lights_key_)state_.emergency_lights=!state_.emergency_lights;
 if(input.siren_pressed&&!previous_siren_key_)state_.siren_enabled=!state_.siren_enabled;
 previous_lights_key_=input.emergency_lights_pressed; previous_siren_key_=input.siren_pressed;
 if(dt<=0.0)return;
 if(input.throttle&&!input.brake_or_reverse) {
  if(state_.speed_mps<0.0)state_.speed_mps=std::min(0.0,state_.speed_mps+service_braking*dt);
  else state_.speed_mps=std::min(max_forward_speed,state_.speed_mps+acceleration*dt);
 } else if(input.brake_or_reverse) {
  if(state_.speed_mps>0.25)state_.speed_mps=std::max(0.0,state_.speed_mps-service_braking*dt);
  else state_.speed_mps=std::max(-max_reverse_speed,state_.speed_mps-reverse_acceleration*dt);
 } else {
  state_.speed_mps=approach_zero(state_.speed_mps,(input.handbrake?handbrake_drag:rolling_drag)*dt);
 }
 if(input.handbrake&&!input.brake_or_reverse)state_.speed_mps=approach_zero(state_.speed_mps,handbrake_drag*dt);
 const double steering=(input.steer_right?1.0:0.0)-(input.steer_left?1.0:0.0);
 const double steering_factor=std::clamp(std::abs(state_.speed_mps)/4.0,0.0,1.0);
 if(steering!=0.0&&steering_factor>0.0) {
  const double reverse_direction=state_.speed_mps<0.0?-1.0:1.0;
  state_.heading_degrees+=steering*reverse_direction*steering_factor*std::abs(state_.speed_mps)*steering_gain*degrees_per_radian*dt;
  while(state_.heading_degrees>=180.0)state_.heading_degrees-=360.0;
  while(state_.heading_degrees< -180.0)state_.heading_degrees+=360.0;
 }
 const double a=state_.heading_degrees/degrees_per_radian;
 const double dx=std::sin(a)*state_.speed_mps*dt,dz=-std::cos(a)*state_.speed_mps*dt;
 if(collision_world!=nullptr){
  const auto resolved=collision_world->move_box({state_.x,0.75,state_.z},{0.95,0.75,2.10},{dx,0.0,dz});
  state_.x=resolved.position.x;state_.z=resolved.position.z;
  if(resolved.collided())state_.speed_mps=0.0;
 }else{state_.x+=dx;state_.z+=dz;}
}
} // namespace police::vehicles

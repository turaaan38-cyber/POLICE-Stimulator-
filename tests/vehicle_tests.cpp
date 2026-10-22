#include "police/vehicles/VehicleController.hpp"
#include <cmath>
#include <iostream>
namespace { int failures=0; void check(bool ok,const char* label){if(ok)std::cout<<"[PASS] "<<label<<'\n';else{std::cerr<<"[FAIL] "<<label<<'\n';++failures;}} }
int main(){
 using police::vehicles::PatrolVehicleController; using police::vehicles::VehicleInput;
 { PatrolVehicleController car;double z0=car.state().z;VehicleInput in;in.throttle=true;for(int i=0;i<100;++i)car.update(in,.05);
   check(car.state().speed_mps>5.0,"throttle accelerates vehicle");check(car.state().speed_mps<=38.0,"forward speed capped");check(car.state().z<z0,"vehicle advances");}
 { PatrolVehicleController car;VehicleInput in;in.throttle=true;for(int i=0;i<30;++i)car.update(in,.05);double v=car.state().speed_mps;
   VehicleInput brake;brake.brake_or_reverse=true;car.update(brake,.05);check(car.state().speed_mps<v,"braking reduces speed");}
 { PatrolVehicleController car;VehicleInput in;in.brake_or_reverse=true;for(int i=0;i<100;++i)car.update(in,.05);
   check(car.state().speed_mps<0.0,"reverse after stopping");check(car.state().speed_mps>=-8.0,"reverse speed capped");}
 { PatrolVehicleController car;VehicleInput in;in.throttle=true;for(int i=0;i<30;++i)car.update(in,.05);in.steer_right=true;for(int i=0;i<20;++i)car.update(in,.05);
   check(car.state().heading_degrees>0.0,"steering changes heading");check(std::abs(car.state().heading_degrees)<=180.0,"heading normalized");}
 { PatrolVehicleController car;VehicleInput in;in.emergency_lights_pressed=true;car.update(in,.016);check(car.state().emergency_lights,"lights toggle on");
   for(int i=0;i<10;++i)car.update(in,.016);check(car.state().emergency_lights,"held light key does not retrigger");
   car.update(VehicleInput{},.016);car.update(in,.016);check(!car.state().emergency_lights,"lights toggle off on next press");}
 { PatrolVehicleController car;VehicleInput in;in.siren_pressed=true;car.update(in,.016);check(car.state().siren_enabled,"siren state toggles on");
   car.update(in,.016);check(car.state().siren_enabled,"held siren key does not retrigger");
   car.update(VehicleInput{},.016);car.update(in,.016);check(!car.state().siren_enabled,"siren state toggles off");}
 { PatrolVehicleController car;VehicleInput in;in.throttle=true;double x=car.state().x,z=car.state().z;car.update(in,100.0);
   check(car.state().speed_mps<1.0,"large delta is clamped");check(std::abs(car.state().x-x)<.1&&std::abs(car.state().z-z)<.1,"large delta cannot teleport vehicle");}
 if(failures){std::cerr<<failures<<" vehicle test(s) failed\n";return 1;}std::cout<<"All vehicle controller tests passed\n";return 0;
}

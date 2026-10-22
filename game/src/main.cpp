#include "police/core/Log.hpp"
#include "police/platform/Window.hpp"
#include "police/renderer/SceneRenderer.hpp"
#include <GLFW/glfw3.h>
#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
int main(int argc,char**argv){
 if(argc>1){const std::string arg=argv[1];if(arg=="--help"||arg=="-h"){
  std::cout<<"POLICE-Stimulator 0.2.0 vehicle blockout\nW: throttle | S: brake/reverse | A/D: steer | Space: handbrake\nL: emergency lights | H: siren state | arrow keys: orbit camera | Escape: quit\nArcade prototype; collision and siren audio are not implemented yet.\n";return 0;}
  if(arg=="--version"){std::cout<<"POLICE-Stimulator 0.2.0-dev\n";return 0;}std::cerr<<"Unknown argument: "<<arg<<" (use --help)\n";return 2;}
 try{
  police::core::log(police::core::LogLevel::Info,"Starting POLICE-Stimulator 0.2.0-dev");
  police::platform::Window window(1280,720,"POLICE-Stimulator | patrol vehicle blockout");police::renderer::SceneRenderer renderer;
  double previous=window.time_seconds(),elapsed=0.0;unsigned frames=0;
  while(!window.should_close()){
   window.poll_events();if(window.key_down(GLFW_KEY_ESCAPE))window.request_close();
   const double now=window.time_seconds(),dt=now-previous;previous=now;
   renderer.update(window,dt);renderer.draw(window);window.swap_buffers();++frames;elapsed+=dt;
   if(elapsed>=1.0){const double fps=double(frames)/elapsed;const auto&v=renderer.vehicle_state();std::ostringstream title;
    title<<"POLICE-Stimulator | "<<std::fixed<<std::setprecision(0)<<fps<<" FPS | "<<std::setprecision(1)<<std::abs(v.speed_mps)*3.6
    <<" km/h | Lights: "<<(v.emergency_lights?"ON":"OFF")<<" | Siren: "<<(v.siren_enabled?"ON":"OFF")<<" | W/S A/D Space L H";
    window.set_title(title.str());elapsed=0.0;frames=0;}
  }
  police::core::log(police::core::LogLevel::Info,"Viewer closed cleanly");
 }catch(const std::exception&e){police::core::log(police::core::LogLevel::Error,e.what());return 1;}
 return 0;
}

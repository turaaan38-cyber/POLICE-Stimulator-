#include "police/physics/PhysicsWorld.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
namespace {int failures=0;void check(bool ok,const char*label){if(ok)std::cout<<"[PASS] "<<label<<'\\n';else{std::cerr<<"[FAIL] "<<label<<'\\n';++failures;}}
template<class F>void invalid(F&&f,const char*label){try{f();check(false,label);}catch(const std::invalid_argument&){check(true,label);}catch(...){check(false,label);}}}
int main(){using namespace police::physics;PhysicsWorld w;w.add_static_box({{5,1,0},{1,1,1}});
check(w.static_box_count()==1,"stores static collider");
check(!w.overlaps({{4.5,1,0},{.5,.5,.5}}),"touching faces are not penetrating");
check(w.overlaps({{4.6,1,0},{.5,.5,.5}}),"detects AABB penetration");
{auto m=w.move_box({0,1,0},{.5,.5,.5},{10,0,0});check(m.hit_x,"reports blocked axis");check(std::abs(m.position.x-3.5)<1e-8,"prevents tunneling through wall");}
{auto m=w.move_box({0,1,0},{.5,.5,.5},{0,0,5});check(!m.collided()&&std::abs(m.position.z-5)<1e-8,"unobstructed movement unchanged");}
{auto m=w.move_box({3.5,1,0},{.5,.5,.5},{0,0,3});check(!m.hit_z&&std::abs(m.position.z-3)<1e-8,"allows sliding along touching wall");}
{auto m=w.move_box({8,1,0},{.5,.5,.5},{-10,0,0});check(m.hit_x&&std::abs(m.position.x-6.5)<1e-8,"resolves motion from opposite direction");}
invalid([&]{w.add_static_box({{0,0,0},{1,0,1}});},"rejects zero-size collider");
invalid([&]{(void)w.move_box({0,0,0},{1,1,1},{NAN,0,0});},"rejects non-finite movement");
w.clear_static();check(w.static_box_count()==0&&!w.overlaps({{0,0,0},{1,1,1}}),"clears collision world");
if(failures){std::cerr<<failures<<" physics test(s) failed\\n";return 1;}std::cout<<"All physics tests passed\\n";return 0;}

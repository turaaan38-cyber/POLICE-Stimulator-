#include "police/physics/PhysicsWorld.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace police::physics { namespace {
constexpr double epsilon=1.0e-9;
bool finite(const Vec3& v){return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
bool valid_half(const Vec3& v){return finite(v)&&v.x>0.0&&v.y>0.0&&v.z>0.0;}
double get(const Vec3& v,int a){return a==0?v.x:(a==1?v.y:v.z);}
double& get(Vec3& v,int a){return a==0?v.x:(a==1?v.y:v.z);}
bool interval_overlap(double a,double ah,double b,double bh){return std::abs(a-b)<ah+bh-epsilon;}
bool intersects(const Aabb&a,const Aabb&b){
 return interval_overlap(a.center.x,a.half_extents.x,b.center.x,b.half_extents.x)&&
 interval_overlap(a.center.y,a.half_extents.y,b.center.y,b.half_extents.y)&&
 interval_overlap(a.center.z,a.half_extents.z,b.center.z,b.half_extents.z);
}
}
void PhysicsWorld::add_static_box(const Aabb& box){
 if(!finite(box.center)||!valid_half(box.half_extents))throw std::invalid_argument("static collider requires finite coordinates and positive half-extents");
 static_boxes_.push_back(box);
}
void PhysicsWorld::clear_static() noexcept{static_boxes_.clear();}
bool PhysicsWorld::overlaps(const Aabb& box)const{
 if(!finite(box.center)||!valid_half(box.half_extents))throw std::invalid_argument("query box requires finite coordinates and positive half-extents");
 return std::any_of(static_boxes_.begin(),static_boxes_.end(),[&](const Aabb&other){return intersects(box,other);});
}
MoveResult PhysicsWorld::move_box(Vec3 center,Vec3 half,Vec3 displacement)const{
 if(!finite(center)||!finite(displacement)||!valid_half(half))throw std::invalid_argument("move_box requires finite coordinates and positive half-extents");
 MoveResult result;result.position=center;
 // Axis-separated resolution prevents tunneling through static boxes and permits wall sliding.
 for(int axis : {0,2,1}){
  const double delta=get(displacement,axis);if(std::abs(delta)<=epsilon)continue;
  const double start=get(result.position,axis);double target=start+delta;
  for(const Aabb& wall:static_boxes_){
   bool other_axes=true;
   for(int other=0;other<3;++other){if(other==axis)continue;
    if(!interval_overlap(get(result.position,other),get(half,other),get(wall.center,other),get(wall.half_extents,other))){other_axes=false;break;}}
   if(!other_axes)continue;
   const double wall_center=get(wall.center,axis),wall_half=get(wall.half_extents,axis),moving_half=get(half,axis);
   const double wall_min=wall_center-wall_half,wall_max=wall_center+wall_half;
   if(delta>0.0&&start+moving_half<=wall_min+epsilon&&target+moving_half>wall_min){
    target=std::min(target,wall_min-moving_half);
    if(axis==0)result.hit_x=true;else if(axis==1)result.hit_y=true;else result.hit_z=true;
   }else if(delta<0.0&&start-moving_half>=wall_max-epsilon&&target-moving_half<wall_max){
    target=std::max(target,wall_max+moving_half);
    if(axis==0)result.hit_x=true;else if(axis==1)result.hit_y=true;else result.hit_z=true;
   }
  }
  get(result.position,axis)=target;
 }
 return result;
}
} // namespace police::physics

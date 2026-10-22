#include "police/renderer/SceneRenderer.hpp"
#include "police/core/Log.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
namespace police::renderer { namespace {
constexpr double pi=3.14159265358979323846;
constexpr GLenum array_buffer=0x8892,static_draw=0x88E4,vertex_shader=0x8B31,fragment_shader=0x8B30;
constexpr GLenum compile_status=0x8B81,link_status=0x8B82,info_length=0x8B84,triangles=0x0004;
struct Vertex{float x,y,z,r,g,b,emergency;};using Matrix=std::array<float,16>;
Matrix identity(){return {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};}
Matrix multiply(const Matrix&a,const Matrix&b){Matrix r{};for(int c=0;c<4;++c)for(int row=0;row<4;++row)for(int k=0;k<4;++k)r[c*4+row]+=a[k*4+row]*b[c*4+k];return r;}
Matrix translation(double x,double y,double z){auto r=identity();r[12]=float(x);r[13]=float(y);r[14]=float(z);return r;}
Matrix rotate_x(double d){double a=d*pi/180;float c=float(cos(a)),s=float(sin(a));return {1,0,0,0,0,c,s,0,0,-s,c,0,0,0,0,1};}
Matrix rotate_y(double d){double a=d*pi/180;float c=float(cos(a)),s=float(sin(a));return {c,0,-s,0,0,1,0,0,s,0,c,0,0,0,0,1};}
Matrix perspective(double aspect){constexpr double n=.1,f=500.;double q=1./tan(70.*pi/360.);Matrix r{};r[0]=float(q/aspect);r[5]=float(q);r[10]=float((f+n)/(n-f));r[11]=-1;r[14]=float(2*f*n/(n-f));return r;}
void triangle(std::vector<Vertex>&v,std::array<float,3>a,std::array<float,3>b,std::array<float,3>c,std::array<float,3>color,bool emergency=false){
 const float e=emergency?1.F:0.F;
 v.push_back({a[0],a[1],a[2],color[0],color[1],color[2],e});
 v.push_back({b[0],b[1],b[2],color[0],color[1],color[2],e});
 v.push_back({c[0],c[1],c[2],color[0],color[1],color[2],e});
}
void quad(std::vector<Vertex>&v,std::array<float,3>a,std::array<float,3>b,std::array<float,3>c,std::array<float,3>d,std::array<float,3>color,bool emergency=false){
 triangle(v,a,b,c,color,emergency);triangle(v,a,c,d,color,emergency);
}
void surface(std::vector<Vertex>&v,double x1,double z1,double x2,double z2,float r,float g,float b,double y){
 quad(v,{float(x1),float(y),float(z1)},{float(x2),float(y),float(z1)},{float(x2),float(y),float(z2)},{float(x1),float(y),float(z2)},{r,g,b});
}
void box(std::vector<Vertex>&v,double x,double y,double z,double w,double h,double d,float r,float g,float b,bool emergency=false){
 float x0=float(x-w*.5),x1=float(x+w*.5),y0=float(y),y1=float(y+h),z0=float(z-d*.5),z1=float(z+d*.5);
 quad(v,{x0,y0,z0},{x1,y0,z0},{x1,y1,z0},{x0,y1,z0},{r*.70F,g*.70F,b*.70F},emergency);
 quad(v,{x0,y0,z1},{x0,y1,z1},{x1,y1,z1},{x1,y0,z1},{r*.82F,g*.82F,b*.82F},emergency);
 quad(v,{x0,y0,z0},{x0,y1,z0},{x0,y1,z1},{x0,y0,z1},{r*.59F,g*.59F,b*.59F},emergency);
 quad(v,{x1,y0,z0},{x1,y0,z1},{x1,y1,z1},{x1,y1,z0},{r*.90F,g*.90F,b*.90F},emergency);
 quad(v,{x0,y1,z0},{x1,y1,z0},{x1,y1,z1},{x0,y1,z1},{r,g,b},emergency);
 quad(v,{x0,y0,z0},{x0,y0,z1},{x1,y0,z1},{x1,y0,z0},{r*.48F,g*.48F,b*.48F},emergency);
}
std::vector<Vertex> build_city(){
 std::vector<Vertex>v;v.reserve(75000);surface(v,-100,-100,100,100,.17F,.28F,.20F,0);
 for(int road=-3;road<=3;++road){double c=road*24.;surface(v,c-4.5,-100,c+4.5,100,.20F,.21F,.23F,.008);
  surface(v,-100,c-4.5,100,c+4.5,.20F,.21F,.23F,.009);
  for(int m=-8;m<=8;++m){double a=m*11.;surface(v,c-.10,a-2,c+.10,a+2,.83F,.78F,.48F,.025);surface(v,a-2,c-.10,a+2,c+.10,.83F,.78F,.48F,.026);}}
 for(int xb=-3;xb<=2;++xb)for(int zb=-3;zb<=2;++zb){double x=xb*24.+12,z=zb*24.+12;surface(v,x-7,z-7,x+7,z+7,.32F,.34F,.31F,.012);
  int seed=(xb+4)*17+(zb+4)*29;double ha=5.+(seed*7)%13,hb=6.+(seed*3+5)%17;float t=.44F+float(seed%4)*.035F;
  box(v,x-3,0,z-2.6,5.4,ha,5.8,t,t*.95F,t*.87F);box(v,x+3.2,0,z+2.8,4.2,hb,4,t*.78F,t*.85F,t);
  for(int floor=1;floor<int(ha/2.4);++floor)for(int wi=-1;wi<=1;++wi)box(v,x-3+wi*1.35,double(floor)*2.25,z-5.53,.60,.72,.05,.22F,.49F,.57F);}
 return v;
}
std::vector<Vertex> build_patrol_car(){
 std::vector<Vertex>v;v.reserve(1500);
 box(v,0,.08,0,1.82,.65,4.15,.78F,.81F,.83F);box(v,0,.53,-1.22,1.72,.20,1.34,.80F,.83F,.85F);
 box(v,0,.63,.20,1.53,.86,1.84,.04F,.10F,.15F);box(v,0,1.20,-.52,1.36,.48,.05,.08F,.25F,.32F);
 box(v,0,1.20,.98,1.34,.45,.05,.08F,.25F,.32F);box(v,0,1.36,.20,1.48,.12,1.78,.79F,.82F,.83F);
 for(int side:{-1,1}){for(int axle:{-1,1})box(v,side*.91,.12,axle*1.34,.22,.42,.64,.035F,.04F,.045F);
  box(v,side*.925,.48,.05,.025,.16,1.25,.025F,.12F,.20F);}
 box(v,-.55,.30,-2.09,.30,.17,.05,.92F,.83F,.52F);box(v,.55,.30,-2.09,.30,.17,.05,.92F,.83F,.52F);
 box(v,0,1.48,.08,.80,.10,.24,.10F,.13F,.17F);
 box(v,-.23,1.54,.08,.30,.08,.18,.16F,.08F,.09F,true);box(v,.23,1.54,.08,.30,.08,.18,.08F,.10F,.22F,true);
 return v;
}
GLuint compile(GLApi&g,GLenum type,const char*src){GLuint s=g.CreateShader(type);g.ShaderSource(s,1,&src,nullptr);g.CompileShader(s);GLint ok=0;g.GetShaderiv(s,compile_status,&ok);if(!ok){GLint n=0;g.GetShaderiv(s,info_length,&n);std::string msg(std::size_t(std::max(1,n)),'\0');g.GetShaderInfoLog(s,GLsizei(msg.size()),nullptr,msg.data());g.DeleteShader(s);throw std::runtime_error("shader compilation failed: "+msg);}return s;}
GLuint make_program(GLApi&g){
 const char*vs="#version 330 core\nlayout(location=0) in vec3 p;layout(location=1) in vec3 c;layout(location=2) in float e;uniform mat4 m;out vec3 v;out float emergency;void main(){v=c;emergency=e;gl_Position=m*vec4(p,1.0);}";
 const char*fs="#version 330 core\nin vec3 v;in float emergency;uniform float lightPhase;uniform float lightsEnabled;out vec4 o;void main(){vec3 c=v;if(emergency>0.5&&lightsEnabled>0.5){if(mod(floor(lightPhase*4.0),2.0)<1.0)c=vec3(1.0,0.025,0.025);else c=vec3(0.025,0.12,1.0);}o=vec4(c,1.0);}";
 GLuint a=compile(g,vertex_shader,vs),b=compile(g,fragment_shader,fs),p=g.CreateProgram();g.AttachShader(p,a);g.AttachShader(p,b);g.LinkProgram(p);g.DeleteShader(a);g.DeleteShader(b);
 GLint ok=0;g.GetProgramiv(p,link_status,&ok);if(!ok){GLint n=0;g.GetProgramiv(p,info_length,&n);std::string msg(std::size_t(std::max(1,n)),'\0');g.GetProgramInfoLog(p,GLsizei(msg.size()),nullptr,msg.data());g.DeleteProgram(p);throw std::runtime_error("shader program link failed: "+msg);}return p;
}
void upload_mesh(GLApi&g,const std::vector<Vertex>&v,GLuint&vao,GLuint&vbo,GLsizei&count){
 if(v.empty()||v.size()>std::size_t(std::numeric_limits<GLsizei>::max()))throw std::runtime_error("invalid mesh vertex count");
 count=GLsizei(v.size());g.GenVertexArrays(1,&vao);g.BindVertexArray(vao);g.GenBuffers(1,&vbo);g.BindBuffer(array_buffer,vbo);g.BufferData(array_buffer,GLsizeiptr(v.size()*sizeof(Vertex)),v.data(),static_draw);
 g.EnableVertexAttribArray(0);g.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);
 g.EnableVertexAttribArray(1);g.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(3*sizeof(float)));
 g.EnableVertexAttribArray(2);g.VertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(6*sizeof(float)));g.BindVertexArray(0);
}
}
SceneRenderer::SceneRenderer(const police::platform::Window& window){
 gl_.load(window);program_=make_program(gl_);uniform_mvp_=gl_.GetUniformLocation(program_,"m");uniform_light_phase_=gl_.GetUniformLocation(program_,"lightPhase");uniform_lights_enabled_=gl_.GetUniformLocation(program_,"lightsEnabled");
 upload_mesh(gl_,build_city(),city_vao_,city_vbo_,city_vertex_count_);
 for(int xb=-3;xb<=2;++xb)for(int zb=-3;zb<=2;++zb){
  const double x=xb*24.0+12.0,z=zb*24.0+12.0;
  const int seed=(xb+4)*17+(zb+4)*29;
  const double ha=5.0+(seed*7)%13,hb=6.0+(seed*3+5)%17;
  collision_world_.add_static_box({{x-3.0,ha*0.5,z-2.6},{2.7,ha*0.5,2.9}});
  collision_world_.add_static_box({{x+3.2,hb*0.5,z+2.8},{2.1,hb*0.5,2.0}});
 }
 upload_mesh(gl_,build_patrol_car(),vehicle_vao_,vehicle_vbo_,vehicle_vertex_count_);
 police::core::log(police::core::LogLevel::Info,"OpenGL 3.3 renderer initialized; city vertices: "+std::to_string(city_vertex_count_));
}
SceneRenderer::~SceneRenderer(){if(city_vbo_)gl_.DeleteBuffers(1,&city_vbo_);if(city_vao_)gl_.DeleteVertexArrays(1,&city_vao_);if(vehicle_vbo_)gl_.DeleteBuffers(1,&vehicle_vbo_);if(vehicle_vao_)gl_.DeleteVertexArrays(1,&vehicle_vao_);if(program_)gl_.DeleteProgram(program_);}
void SceneRenderer::update(const police::platform::Window&w,double dt){
 police::vehicles::VehicleInput input;input.throttle=w.key_down(police::platform::Key::W);input.brake_or_reverse=w.key_down(police::platform::Key::S);
 input.steer_left=w.key_down(police::platform::Key::A);input.steer_right=w.key_down(police::platform::Key::D);input.handbrake=w.key_down(police::platform::Key::SPACE);
 input.emergency_lights_pressed=w.key_down(police::platform::Key::L);input.siren_pressed=w.key_down(police::platform::Key::H);vehicle_.update(input,dt,&collision_world_);
 const double s=std::clamp(dt,0.0,0.05);
 if(w.key_down(police::platform::Key::LEFT))camera_orbit_degrees_-=65.0*s;if(w.key_down(police::platform::Key::Right))camera_orbit_degrees_+=65.0*s;
 if(w.key_down(police::platform::Key::Up))pitch_degrees_=std::max(4.0,pitch_degrees_-35.0*s);if(w.key_down(police::platform::Key::DOWN))pitch_degrees_=std::min(45.0,pitch_degrees_+35.0*s);
 while(camera_orbit_degrees_>=180.0)camera_orbit_degrees_-=360.0;while(camera_orbit_degrees_<-180.0)camera_orbit_degrees_+=360.0;
}
void SceneRenderer::draw(const police::platform::Window&w)const{
 const int width=std::max(1,w.framebuffer_width()),height=std::max(1,w.framebuffer_height());glViewport(0,0,width,height);
 glClearColor(.49F,.67F,.82F,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glEnable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);
 const auto&s=vehicle_.state();const double camera_heading=s.heading_degrees+camera_orbit_degrees_,a=camera_heading*pi/180.0;
 const double cam_x=s.x-std::sin(a)*9.0,cam_z=s.z+std::cos(a)*9.0,cam_y=5.5;
 const Matrix view=multiply(rotate_y(camera_heading),multiply(rotate_x(pitch_degrees_),translation(-cam_x,-cam_y,-cam_z)));
 const Matrix projection=perspective(double(width)/double(height)),city_mvp=multiply(projection,view);
 gl_.UseProgram(program_);gl_.Uniform1f(uniform_light_phase_,GLfloat(w.time_seconds()));gl_.Uniform1f(uniform_lights_enabled_,s.emergency_lights?1.0F:0.0F);
 gl_.UniformMatrix4fv(uniform_mvp_,1,GL_FALSE,city_mvp.data());gl_.BindVertexArray(city_vao_);gl_.DrawArrays(triangles,0,city_vertex_count_);
 const Matrix car_world=multiply(translation(s.x,0.0,s.z),rotate_y(-s.heading_degrees)),car_mvp=multiply(city_mvp,car_world);
 gl_.UniformMatrix4fv(uniform_mvp_,1,GL_FALSE,car_mvp.data());gl_.BindVertexArray(vehicle_vao_);gl_.DrawArrays(triangles,0,vehicle_vertex_count_);gl_.BindVertexArray(0);
}
} // namespace police::renderer

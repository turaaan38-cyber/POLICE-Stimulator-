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
constexpr double pi=3.14159265358979323846;constexpr GLenum array_buffer=0x8892,static_draw=0x88E4,vertex_shader=0x8B31,fragment_shader=0x8B30,compile_status=0x8B81,link_status=0x8B82,info_length=0x8B84,triangles=0x0004;
struct Vertex{float x,y,z,r,g,b;};using Matrix=std::array<float,16>;
Matrix identity(){return {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};}
Matrix multiply(const Matrix&a,const Matrix&b){Matrix r{};for(int c=0;c<4;++c)for(int row=0;row<4;++row)for(int k=0;k<4;++k)r[c*4+row]+=a[k*4+row]*b[c*4+k];return r;}
Matrix translation(double x,double y,double z){auto r=identity();r[12]=float(x);r[13]=float(y);r[14]=float(z);return r;}
Matrix rotate_x(double d){double a=d*pi/180;float c=float(cos(a)),s=float(sin(a));return {1,0,0,0,0,c,s,0,0,-s,c,0,0,0,0,1};}
Matrix rotate_y(double d){double a=d*pi/180;float c=float(cos(a)),s=float(sin(a));return {c,0,-s,0,0,1,0,0,s,0,c,0,0,0,0,1};}
Matrix perspective(double aspect){constexpr double n=.1,f=500.;double q=1./tan(70.*pi/360.);Matrix r{};r[0]=float(q/aspect);r[5]=float(q);r[10]=float((f+n)/(n-f));r[11]=-1;r[14]=float(2*f*n/(n-f));return r;}
void triangle(std::vector<Vertex>&v,std::array<float,3>a,std::array<float,3>b,std::array<float,3>c,std::array<float,3>color){v.push_back({a[0],a[1],a[2],color[0],color[1],color[2]});v.push_back({b[0],b[1],b[2],color[0],color[1],color[2]});v.push_back({c[0],c[1],c[2],color[0],color[1],color[2]});}
void quad(std::vector<Vertex>&v,std::array<float,3>a,std::array<float,3>b,std::array<float,3>c,std::array<float,3>d,std::array<float,3>color){triangle(v,a,b,c,color);triangle(v,a,c,d,color);}
void surface(std::vector<Vertex>&v,double x1,double z1,double x2,double z2,float r,float g,float b,double y){quad(v,{float(x1),float(y),float(z1)},{float(x2),float(y),float(z1)},{float(x2),float(y),float(z2)},{float(x1),float(y),float(z2)},{r,g,b});}
void box(std::vector<Vertex>&v,double x,double y,double z,double w,double h,double d,float r,float g,float b){
 float x0=float(x-w*.5),x1=float(x+w*.5),y0=float(y),y1=float(y+h),z0=float(z-d*.5),z1=float(z+d*.5);
 quad(v,{x0,y0,z0},{x1,y0,z0},{x1,y1,z0},{x0,y1,z0},{r*.70F,g*.70F,b*.70F});quad(v,{x0,y0,z1},{x0,y1,z1},{x1,y1,z1},{x1,y0,z1},{r*.82F,g*.82F,b*.82F});
 quad(v,{x0,y0,z0},{x0,y1,z0},{x0,y1,z1},{x0,y0,z1},{r*.59F,g*.59F,b*.59F});quad(v,{x1,y0,z0},{x1,y0,z1},{x1,y1,z1},{x1,y1,z0},{r*.90F,g*.90F,b*.90F});
 quad(v,{x0,y1,z0},{x1,y1,z0},{x1,y1,z1},{x0,y1,z1},{r,g,b});quad(v,{x0,y0,z0},{x0,y0,z1},{x1,y0,z1},{x1,y0,z0},{r*.48F,g*.48F,b*.48F});}
std::vector<Vertex> build_blockout(){std::vector<Vertex>v;v.reserve(75000);surface(v,-100,-100,100,100,.17F,.28F,.20F,0);
 for(int road=-3;road<=3;++road){double c=road*24.;surface(v,c-4.5,-100,c+4.5,100,.20F,.21F,.23F,.008);surface(v,-100,c-4.5,100,c+4.5,.20F,.21F,.23F,.009);for(int m=-8;m<=8;++m){double a=m*11.;surface(v,c-.10,a-2,c+.10,a+2,.83F,.78F,.48F,.025);surface(v,a-2,c-.10,a+2,c+.10,.83F,.78F,.48F,.026);}}
 for(int xb=-3;xb<=2;++xb)for(int zb=-3;zb<=2;++zb){double x=xb*24.+12,z=zb*24.+12;surface(v,x-7,z-7,x+7,z+7,.32F,.34F,.31F,.012);int seed=(xb+4)*17+(zb+4)*29;double ha=5.+(seed*7)%13,hb=6.+(seed*3+5)%17;float t=.44F+float(seed%4)*.035F;box(v,x-3,0,z-2.6,5.4,ha,5.8,t,t*.95F,t*.87F);box(v,x+3.2,0,z+2.8,4.2,hb,4,t*.78F,t*.85F,t);for(int floor=1;floor<int(ha/2.4);++floor)for(int wi=-1;wi<=1;++wi)box(v,x-3+wi*1.35,double(floor)*2.25,z-5.53,.60,.72,.05,.22F,.49F,.57F);}
 box(v,2.1,.28,-3,1.55,.62,3,.81F,.83F,.84F);box(v,2.1,.77,-3.05,1.18,.48,1.48,.12F,.19F,.24F);box(v,2.1,.79,-3.98,1.2,.2,.14,.72F,.74F,.76F);box(v,2.1,.79,-2.12,1.2,.2,.14,.72F,.74F,.76F);box(v,1.75,1.12,-3.05,.42,.12,.2,.90F,.08F,.08F);box(v,2.45,1.12,-3.05,.42,.12,.2,.08F,.20F,.95F);return v;}
GLuint compile(GLApi&g,GLenum type,const char*src){GLuint s=g.CreateShader(type);g.ShaderSource(s,1,&src,nullptr);g.CompileShader(s);GLint ok=0;g.GetShaderiv(s,compile_status,&ok);if(!ok){GLint n=0;g.GetShaderiv(s,info_length,&n);std::string msg(std::size_t(std::max(1,n)),'\0');g.GetShaderInfoLog(s,GLsizei(msg.size()),nullptr,msg.data());g.DeleteShader(s);throw std::runtime_error("shader compilation failed: "+msg);}return s;}
GLuint make_program(GLApi&g){const char*vs="#version 330 core\nlayout(location=0) in vec3 p;layout(location=1) in vec3 c;uniform mat4 m;out vec3 v;void main(){v=c;gl_Position=m*vec4(p,1.0);}";const char*fs="#version 330 core\nin vec3 v;out vec4 o;void main(){o=vec4(v,1.0);}";GLuint a=compile(g,vertex_shader,vs),b=compile(g,fragment_shader,fs),p=g.CreateProgram();g.AttachShader(p,a);g.AttachShader(p,b);g.LinkProgram(p);g.DeleteShader(a);g.DeleteShader(b);GLint ok=0;g.GetProgramiv(p,link_status,&ok);if(!ok){GLint n=0;g.GetProgramiv(p,info_length,&n);std::string msg(std::size_t(std::max(1,n)),'\0');g.GetProgramInfoLog(p,GLsizei(msg.size()),nullptr,msg.data());g.DeleteProgram(p);throw std::runtime_error("shader program link failed: "+msg);}return p;}
}
SceneRenderer::SceneRenderer(){gl_.load();program_=make_program(gl_);uniform_mvp_=gl_.GetUniformLocation(program_,"m");auto v=build_blockout();if(v.empty())throw std::runtime_error("procedural blockout mesh is empty");if(v.size()>std::size_t(std::numeric_limits<GLsizei>::max()))throw std::runtime_error("procedural blockout mesh too large");vertex_count_=GLsizei(v.size());gl_.GenVertexArrays(1,&vao_);gl_.BindVertexArray(vao_);gl_.GenBuffers(1,&vbo_);gl_.BindBuffer(array_buffer,vbo_);gl_.BufferData(array_buffer,GLsizeiptr(v.size()*sizeof(Vertex)),v.data(),static_draw);gl_.EnableVertexAttribArray(0);gl_.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),nullptr);gl_.EnableVertexAttribArray(1);gl_.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(3*sizeof(float)));gl_.BindVertexArray(0);police::core::log(police::core::LogLevel::Info,"OpenGL 3.3 renderer initialized; blockout vertices: "+std::to_string(vertex_count_));}
SceneRenderer::~SceneRenderer(){if(vbo_)gl_.DeleteBuffers(1,&vbo_);if(vao_)gl_.DeleteVertexArrays(1,&vao_);if(program_)gl_.DeleteProgram(program_);}
void SceneRenderer::update(const police::platform::Window&w,double dt){double s=std::clamp(dt,0.,.05),a=yaw_degrees_*pi/180.,fx=sin(a),fz=-cos(a),rx=cos(a),rz=sin(a),speed=w.key_down(GLFW_KEY_LEFT_SHIFT)?16.:7.;if(w.key_down(GLFW_KEY_W)){camera_x_+=fx*speed*s;camera_z_+=fz*speed*s;}if(w.key_down(GLFW_KEY_S)){camera_x_-=fx*speed*s;camera_z_-=fz*speed*s;}if(w.key_down(GLFW_KEY_D)){camera_x_+=rx*speed*s;camera_z_+=rz*speed*s;}if(w.key_down(GLFW_KEY_A)){camera_x_-=rx*speed*s;camera_z_-=rz*speed*s;}if(w.key_down(GLFW_KEY_UP))pitch_degrees_=std::min(pitch_degrees_+65*s,80.);if(w.key_down(GLFW_KEY_DOWN))pitch_degrees_=std::max(pitch_degrees_-65*s,-80.);if(w.key_down(GLFW_KEY_LEFT))yaw_degrees_-=80*s;if(w.key_down(GLFW_KEY_RIGHT))yaw_degrees_+=80*s;}
void SceneRenderer::draw(const police::platform::Window&w)const{int width=std::max(1,w.framebuffer_width()),height=std::max(1,w.framebuffer_height());glViewport(0,0,width,height);glClearColor(.49F,.67F,.82F,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);glEnable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);auto view=multiply(rotate_y(yaw_degrees_),multiply(rotate_x(pitch_degrees_),translation(-camera_x_,-camera_y_,-camera_z_)));auto mvp=multiply(perspective(double(width)/height),view);gl_.UseProgram(program_);gl_.UniformMatrix4fv(uniform_mvp_,1,GL_FALSE,mvp.data());gl_.BindVertexArray(vao_);gl_.DrawArrays(triangles,0,vertex_count_);gl_.BindVertexArray(0);}
} // namespace police::renderer

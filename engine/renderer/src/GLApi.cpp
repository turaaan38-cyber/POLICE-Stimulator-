#include "police/renderer/GLApi.hpp"
#include <stdexcept>
#include <string>
namespace police::renderer { namespace {
template<typename Function> Function load_function(const police::platform::Window& window,const char* name) {
 auto address=window.get_proc_address(name);
 if(!address)throw std::runtime_error(std::string("OpenGL function is unavailable: ")+name);
 return reinterpret_cast<Function>(address);
}}
void GLApi::load(const police::platform::Window& window) {
#define POLICE_LOAD_GL(name) name=load_function<name##Fn>(window,"gl" #name)
 POLICE_LOAD_GL(GenVertexArrays);POLICE_LOAD_GL(BindVertexArray);POLICE_LOAD_GL(DeleteVertexArrays);
 POLICE_LOAD_GL(GenBuffers);POLICE_LOAD_GL(BindBuffer);POLICE_LOAD_GL(BufferData);POLICE_LOAD_GL(DeleteBuffers);
 POLICE_LOAD_GL(EnableVertexAttribArray);POLICE_LOAD_GL(VertexAttribPointer);
 POLICE_LOAD_GL(CreateShader);POLICE_LOAD_GL(ShaderSource);POLICE_LOAD_GL(CompileShader);
 POLICE_LOAD_GL(GetShaderiv);POLICE_LOAD_GL(GetShaderInfoLog);POLICE_LOAD_GL(DeleteShader);
 POLICE_LOAD_GL(CreateProgram);POLICE_LOAD_GL(AttachShader);POLICE_LOAD_GL(LinkProgram);
 POLICE_LOAD_GL(GetProgramiv);POLICE_LOAD_GL(GetProgramInfoLog);POLICE_LOAD_GL(DeleteProgram);
 POLICE_LOAD_GL(UseProgram);POLICE_LOAD_GL(GetUniformLocation);POLICE_LOAD_GL(UniformMatrix4fv);
 POLICE_LOAD_GL(Uniform1f);POLICE_LOAD_GL(DrawArrays);
#undef POLICE_LOAD_GL
}
} // namespace police::renderer

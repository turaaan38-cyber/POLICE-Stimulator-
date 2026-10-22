#pragma once
#include "police/platform/Window.hpp"
#include <cstddef>
#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
namespace police::renderer {
#ifndef APIENTRY
#define APIENTRY
#endif
using GLchar=char;
using GLsizeiptr=std::ptrdiff_t;
struct GLApi final {
 using GenVertexArraysFn=void(APIENTRY*)(GLsizei,GLuint*); using BindVertexArrayFn=void(APIENTRY*)(GLuint);
 using DeleteVertexArraysFn=void(APIENTRY*)(GLsizei,const GLuint*); using GenBuffersFn=void(APIENTRY*)(GLsizei,GLuint*);
 using BindBufferFn=void(APIENTRY*)(GLenum,GLuint); using BufferDataFn=void(APIENTRY*)(GLenum,GLsizeiptr,const void*,GLenum);
 using DeleteBuffersFn=void(APIENTRY*)(GLsizei,const GLuint*); using EnableVertexAttribArrayFn=void(APIENTRY*)(GLuint);
 using VertexAttribPointerFn=void(APIENTRY*)(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*);
 using CreateShaderFn=GLuint(APIENTRY*)(GLenum); using ShaderSourceFn=void(APIENTRY*)(GLuint,GLsizei,const GLchar* const*,const GLint*);
 using CompileShaderFn=void(APIENTRY*)(GLuint); using GetShaderivFn=void(APIENTRY*)(GLuint,GLenum,GLint*);
 using GetShaderInfoLogFn=void(APIENTRY*)(GLuint,GLsizei,GLsizei*,GLchar*); using DeleteShaderFn=void(APIENTRY*)(GLuint);
 using CreateProgramFn=GLuint(APIENTRY*)(); using AttachShaderFn=void(APIENTRY*)(GLuint,GLuint);
 using LinkProgramFn=void(APIENTRY*)(GLuint); using GetProgramivFn=void(APIENTRY*)(GLuint,GLenum,GLint*);
 using GetProgramInfoLogFn=void(APIENTRY*)(GLuint,GLsizei,GLsizei*,GLchar*); using DeleteProgramFn=void(APIENTRY*)(GLuint);
 using UseProgramFn=void(APIENTRY*)(GLuint); using GetUniformLocationFn=GLint(APIENTRY*)(GLuint,const GLchar*);
 using UniformMatrix4fvFn=void(APIENTRY*)(GLint,GLsizei,GLboolean,const GLfloat*);
 using Uniform1fFn=void(APIENTRY*)(GLint,GLfloat); using DrawArraysFn=void(APIENTRY*)(GLenum,GLint,GLsizei);
 GenVertexArraysFn GenVertexArrays{}; BindVertexArrayFn BindVertexArray{}; DeleteVertexArraysFn DeleteVertexArrays{};
 GenBuffersFn GenBuffers{}; BindBufferFn BindBuffer{}; BufferDataFn BufferData{}; DeleteBuffersFn DeleteBuffers{};
 EnableVertexAttribArrayFn EnableVertexAttribArray{}; VertexAttribPointerFn VertexAttribPointer{};
 CreateShaderFn CreateShader{}; ShaderSourceFn ShaderSource{}; CompileShaderFn CompileShader{};
 GetShaderivFn GetShaderiv{}; GetShaderInfoLogFn GetShaderInfoLog{}; DeleteShaderFn DeleteShader{};
 CreateProgramFn CreateProgram{}; AttachShaderFn AttachShader{}; LinkProgramFn LinkProgram{};
 GetProgramivFn GetProgramiv{}; GetProgramInfoLogFn GetProgramInfoLog{}; DeleteProgramFn DeleteProgram{};
 UseProgramFn UseProgram{}; GetUniformLocationFn GetUniformLocation{}; UniformMatrix4fvFn UniformMatrix4fv{};
 Uniform1fFn Uniform1f{}; DrawArraysFn DrawArrays{};
 void load(const police::platform::Window& window);
};
} // namespace police::renderer

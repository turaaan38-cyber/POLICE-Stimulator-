#include "police/platform/Window.hpp"
#include "police/core/Log.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <GL/gl.h>
#define GLX_GLXEXT_PROTOTYPES 1
#include <GL/glx.h>
#include <array>
#include <chrono>
#include <stdexcept>
#include <string>
namespace police::platform { namespace {
constexpr int glx_context_major=0x2091,glx_context_minor=0x2092,glx_context_profile_mask=0x9126,glx_context_core_profile=0x00000001;
using CreateContextAttribs=GLXContext(*)(Display*,GLXFBConfig,GLXContext,Bool,const int*);
constexpr std::size_t index(Key k){return static_cast<std::size_t>(k);}
Key from_keysym(KeySym key) {
 switch(key){
 case XK_w:case XK_W:return Key::W;case XK_a:case XK_A:return Key::A;case XK_s:case XK_S:return Key::S;
 case XK_d:case XK_D:return Key::D;case XK_space:return Key::Space;case XK_Escape:return Key::Escape;
 case XK_Shift_L:case XK_Shift_R:return Key::Shift;case XK_l:case XK_L:return Key::L;case XK_h:case XK_H:return Key::H;
 case XK_Left:return Key::Left;case XK_Right:return Key::Right;case XK_Up:return Key::Up;case XK_Down:return Key::Down;
 default:return Key::Count;
 }
}
}
struct Window::Impl {
 Display* display{}; ::Window handle{}; Colormap colormap{}; GLXContext context{}; Atom wm_delete{};
 bool closing{false}; int width{},height{}; std::array<bool,index(Key::Count)> keys{};
 std::chrono::steady_clock::time_point started{std::chrono::steady_clock::now()};
 ~Impl(){if(display){if(context){glXMakeCurrent(display,None,nullptr);glXDestroyContext(display,context);}if(handle)XDestroyWindow(display,handle);if(colormap)XFreeColormap(display,colormap);XCloseDisplay(display);}}
};
Window::Window(int width,int height,const char* title):impl_(std::make_unique<Impl>()){
 auto&s=*impl_;s.width=width;s.height=height;s.display=XOpenDisplay(nullptr);
 if(!s.display)throw std::runtime_error("cannot open X11 display; install an X server or use XWayland");
 const int screen=DefaultScreen(s.display);
 const int attrs[]={GLX_X_RENDERABLE,True,GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,GLX_RENDER_TYPE,GLX_RGBA_BIT,
 GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,GLX_RED_SIZE,8,GLX_GREEN_SIZE,8,GLX_BLUE_SIZE,8,GLX_ALPHA_SIZE,8,
 GLX_DEPTH_SIZE,24,GLX_DOUBLEBUFFER,True,None};
 int count=0;GLXFBConfig* configs=glXChooseFBConfig(s.display,screen,attrs,&count);
 if(!configs||count==0){if(configs)XFree(configs);throw std::runtime_error("no compatible GLX framebuffer configuration found");}
 GLXFBConfig config=configs[0];XVisualInfo* visual=glXGetVisualFromFBConfig(s.display,config);XFree(configs);
 if(!visual)throw std::runtime_error("GLX did not provide an X11 visual");
 s.colormap=XCreateColormap(s.display,RootWindow(s.display,visual->screen),visual->visual,AllocNone);
 XSetWindowAttributes wa{};wa.colormap=s.colormap;wa.event_mask=ExposureMask|StructureNotifyMask|KeyPressMask|KeyReleaseMask;
 s.handle=XCreateWindow(s.display,RootWindow(s.display,visual->screen),0,0,static_cast<unsigned>(width),static_cast<unsigned>(height),0,
 visual->depth,InputOutput,visual->visual,CWColormap|CWEventMask,&wa);XFree(visual);
 if(!s.handle)throw std::runtime_error("could not create native X11 window");
 XStoreName(s.display,s.handle,title);s.wm_delete=XInternAtom(s.display,"WM_DELETE_WINDOW",False);XSetWMProtocols(s.display,s.handle,&s.wm_delete,1);
 XMapWindow(s.display,s.handle);XFlush(s.display);
 auto create=reinterpret_cast<CreateContextAttribs>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB")));
 if(!create)throw std::runtime_error("OpenGL driver lacks GLX_ARB_create_context");
 const int context_attrs[]={glx_context_major,3,glx_context_minor,3,glx_context_profile_mask,glx_context_core_profile,None};
 s.context=create(s.display,config,nullptr,True,context_attrs);
 if(!s.context)throw std::runtime_error("could not create an OpenGL 3.3 Core context");
 if(!glXMakeCurrent(s.display,s.handle,s.context))throw std::runtime_error("could not activate the OpenGL context");
 police::core::log(police::core::LogLevel::Info,"Native X11/GLX platform initialized");
}
Window::~Window()=default;
bool Window::should_close()const{return impl_->closing;}
bool Window::key_down(Key key)const{auto i=index(key);return i<impl_->keys.size()&&impl_->keys[i];}
void Window::poll_events(){auto&s=*impl_;while(XPending(s.display)>0){XEvent e{};XNextEvent(s.display,&e);
 if(e.type==ClientMessage&&static_cast<Atom>(e.xclient.data.l[0])==s.wm_delete)s.closing=true;
 else if(e.type==DestroyNotify)s.closing=true;
 else if(e.type==ConfigureNotify){s.width=e.xconfigure.width;s.height=e.xconfigure.height;}
 else if(e.type==KeyPress||e.type==KeyRelease){Key k=from_keysym(XLookupKeysym(&e.xkey,0));if(k!=Key::Count)s.keys[index(k)]=(e.type==KeyPress);}
 }}
void Window::swap_buffers()const{glXSwapBuffers(impl_->display,impl_->handle);}
void Window::request_close(){impl_->closing=true;}
int Window::framebuffer_width()const{return impl_->width;}
int Window::framebuffer_height()const{return impl_->height;}
double Window::time_seconds()const{return std::chrono::duration<double>(std::chrono::steady_clock::now()-impl_->started).count();}
void Window::set_title(const std::string&title){XStoreName(impl_->display,impl_->handle,title.c_str());}
void* Window::get_proc_address(const char*name)const{return reinterpret_cast<void*>(glXGetProcAddressARB(reinterpret_cast<const GLubyte*>(name)));}
} // namespace police::platform

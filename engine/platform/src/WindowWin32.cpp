#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#include "police/platform/Window.hpp"
#include "police/core/Log.hpp"
#include <array>
#include <chrono>
#include <stdexcept>
#include <string>
namespace police::platform { namespace {
constexpr int wgl_major=0x2091,wgl_minor=0x2092,wgl_profile_mask=0x9126,wgl_core_profile=0x00000001;
constexpr std::size_t index(Key k){return static_cast<std::size_t>(k);}
struct NativeState {
 HWND hwnd{};HDC dc{};HGLRC context{};bool closing{false};int width{},height{};
 std::array<bool,index(Key::Count)> keys{};std::chrono::steady_clock::time_point started{std::chrono::steady_clock::now()};
};
Key from_virtual_key(WPARAM k){
 switch(k){case 'W':return Key::W;case 'A':return Key::A;case 'S':return Key::S;case 'D':return Key::D;
 case VK_SPACE:return Key::Space;case VK_ESCAPE:return Key::Escape;case VK_SHIFT:case VK_LSHIFT:case VK_RSHIFT:return Key::Shift;
 case 'L':return Key::L;case 'H':return Key::H;case VK_LEFT:return Key::Left;case VK_RIGHT:return Key::Right;
 case VK_UP:return Key::Up;case VK_DOWN:return Key::Down;default:return Key::Count;}
}
LRESULT CALLBACK window_proc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp){
 auto*s=reinterpret_cast<NativeState*>(GetWindowLongPtrW(hwnd,GWLP_USERDATA));
 if(msg==WM_NCCREATE){auto*cs=reinterpret_cast<CREATESTRUCTW*>(lp);s=static_cast<NativeState*>(cs->lpCreateParams);s->hwnd=hwnd;SetWindowLongPtrW(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(s));}
 if(!s)return DefWindowProcW(hwnd,msg,wp,lp);
 switch(msg){
 case WM_SIZE:s->width=LOWORD(lp);s->height=HIWORD(lp);return 0;
 case WM_KEYDOWN:case WM_SYSKEYDOWN:{auto k=from_virtual_key(wp);if(k!=Key::Count)s->keys[index(k)]=true;return 0;}
 case WM_KEYUP:case WM_SYSKEYUP:{auto k=from_virtual_key(wp);if(k!=Key::Count)s->keys[index(k)]=false;return 0;}
 case WM_CLOSE:s->closing=true;PostQuitMessage(0);return 0;
 case WM_DESTROY:s->closing=true;PostQuitMessage(0);return 0;
 default:return DefWindowProcW(hwnd,msg,wp,lp);
 }
}
bool usable_proc(PROC p){const auto n=reinterpret_cast<std::intptr_t>(p);return p&&n!=1&&n!=2&&n!=3&&n!=-1;}
PROC wgl_proc(const char*name){PROC p=wglGetProcAddress(name);if(!usable_proc(p)){static HMODULE module=LoadLibraryW(L"opengl32.dll");p=module?reinterpret_cast<PROC>(GetProcAddress(module,name)):nullptr;}return p;}
}
struct Window::Impl{NativeState state;~Impl(){if(state.context){wglMakeCurrent(nullptr,nullptr);wglDeleteContext(state.context);}if(state.dc&&state.hwnd)ReleaseDC(state.hwnd,state.dc);if(state.hwnd)DestroyWindow(state.hwnd);}};
Window::Window(int width,int height,const char*title):impl_(std::make_unique<Impl>()){
 static const wchar_t* class_name=L"PoliceStimulatorNativeWindow";static bool registered=false;auto&s=impl_->state;
 if(!registered){WNDCLASSEXW wc{};wc.cbSize=sizeof(wc);wc.style=CS_OWNDC;wc.lpfnWndProc=window_proc;wc.hInstance=GetModuleHandleW(nullptr);wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.lpszClassName=class_name;
 if(!RegisterClassExW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)throw std::runtime_error("RegisterClassExW failed");registered=true;}
 const int length=MultiByteToWideChar(CP_UTF8,0,title,-1,nullptr,0);std::wstring wide_title(static_cast<std::size_t>(length>0?length:1),L'\0');
 if(length>0)MultiByteToWideChar(CP_UTF8,0,title,-1,wide_title.data(),length);else wide_title=L"POLICE-Stimulator";
 RECT rect{0,0,width,height};AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
 s.hwnd=CreateWindowExW(0,class_name,wide_title.c_str(),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,rect.right-rect.left,rect.bottom-rect.top,nullptr,nullptr,GetModuleHandleW(nullptr),&s);
 if(!s.hwnd)throw std::runtime_error("CreateWindowExW failed");s.dc=GetDC(s.hwnd);if(!s.dc)throw std::runtime_error("GetDC failed");
 PIXELFORMATDESCRIPTOR pfd{};pfd.nSize=sizeof(pfd);pfd.nVersion=1;pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;pfd.iPixelType=PFD_TYPE_RGBA;pfd.cColorBits=32;pfd.cAlphaBits=8;pfd.cDepthBits=24;pfd.iLayerType=PFD_MAIN_PLANE;
 const int format=ChoosePixelFormat(s.dc,&pfd);if(!format||!SetPixelFormat(s.dc,format,&pfd))throw std::runtime_error("could not choose an OpenGL pixel format");
 HGLRC legacy=wglCreateContext(s.dc);if(!legacy||!wglMakeCurrent(s.dc,legacy))throw std::runtime_error("could not create the bootstrap OpenGL context");
 using CreateContext=HGLRC(WINAPI*)(HDC,HGLRC,const int*);auto create=reinterpret_cast<CreateContext>(wgl_proc("wglCreateContextAttribsARB"));
 if(!create){wglMakeCurrent(nullptr,nullptr);wglDeleteContext(legacy);throw std::runtime_error("OpenGL driver lacks WGL_ARB_create_context");}
 const int attrs[]={wgl_major,3,wgl_minor,3,wgl_profile_mask,wgl_core_profile,0};s.context=create(s.dc,nullptr,attrs);
 wglMakeCurrent(nullptr,nullptr);wglDeleteContext(legacy);
 if(!s.context||!wglMakeCurrent(s.dc,s.context))throw std::runtime_error("could not activate an OpenGL 3.3 Core context");
 ShowWindow(s.hwnd,SW_SHOW);UpdateWindow(s.hwnd);RECT client{};GetClientRect(s.hwnd,&client);s.width=client.right;s.height=client.bottom;
 police::core::log(police::core::LogLevel::Info,"Native Win32/WGL platform initialized");
}
Window::~Window()=default;
bool Window::should_close()const{return impl_->state.closing;}
bool Window::key_down(Key key)const{auto i=index(key);return i<impl_->state.keys.size()&&impl_->state.keys[i];}
void Window::poll_events(){MSG msg{};while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){if(msg.message==WM_QUIT)impl_->state.closing=true;TranslateMessage(&msg);DispatchMessageW(&msg);}}
void Window::swap_buffers()const{if(impl_->state.dc)SwapBuffers(impl_->state.dc);}
void Window::request_close(){impl_->state.closing=true;if(impl_->state.hwnd)PostMessageW(impl_->state.hwnd,WM_CLOSE,0,0);}
int Window::framebuffer_width()const{return impl_->state.width;}
int Window::framebuffer_height()const{return impl_->state.height;}
double Window::time_seconds()const{return std::chrono::duration<double>(std::chrono::steady_clock::now()-impl_->state.started).count();}
void Window::set_title(const std::string&title){const int n=MultiByteToWideChar(CP_UTF8,0,title.c_str(),-1,nullptr,0);std::wstring w(static_cast<std::size_t>(n>0?n:1),L'\0');if(n>0)MultiByteToWideChar(CP_UTF8,0,title.c_str(),-1,w.data(),n);SetWindowTextW(impl_->state.hwnd,w.c_str());}
void* Window::get_proc_address(const char*name)const{return reinterpret_cast<void*>(wgl_proc(name));}
} // namespace police::platform

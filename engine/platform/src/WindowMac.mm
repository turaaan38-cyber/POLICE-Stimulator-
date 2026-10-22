#import <Cocoa/Cocoa.h>
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#include "police/platform/Window.hpp"
#include "police/core/Log.hpp"
#include <array>
#include <chrono>
#include <dlfcn.h>
#include <stdexcept>

namespace police::platform {
constexpr std::size_t index(Key k){return static_cast<std::size_t>(k);}
struct NativeState { std::array<bool,index(Key::Count)> keys{}; bool closing{false}; };
Key from_key_code(unsigned short c){
 switch(c){case 13:return Key::W;case 0:return Key::A;case 1:return Key::S;case 2:return Key::D;case 49:return Key::Space;
 case 53:return Key::Escape;case 56:case 60:return Key::Shift;case 37:return Key::L;case 4:return Key::H;
 case 123:return Key::Left;case 124:return Key::Right;case 126:return Key::Up;case 125:return Key::Down;default:return Key::Count;}
}
} // namespace police::platform

@interface PoliceInputView : NSView { @public void* input_state; }
@end
@implementation PoliceInputView
- (BOOL)acceptsFirstResponder{return YES;}
- (void)keyDown:(NSEvent*)event{
 auto*s=static_cast<police::platform::NativeState*>(input_state);
 if(s){auto k=police::platform::from_key_code([event keyCode]);if(k!=police::platform::Key::Count)s->keys[police::platform::index(k)]=true;}
}
- (void)keyUp:(NSEvent*)event{
 auto*s=static_cast<police::platform::NativeState*>(input_state);
 if(s){auto k=police::platform::from_key_code([event keyCode]);if(k!=police::platform::Key::Count)s->keys[police::platform::index(k)]=false;}
}
@end
@interface PoliceWindowDelegate : NSObject<NSWindowDelegate> { @public void* input_state; }
@end
@implementation PoliceWindowDelegate
- (BOOL)windowShouldClose:(id)sender{
 (void)sender;auto*s=static_cast<police::platform::NativeState*>(input_state);if(s)s->closing=true;return YES;
}
@end

namespace police::platform {
struct Window::Impl {
 NSWindow* window{};NSOpenGLContext* context{};PoliceWindowDelegate* delegate{};NativeState state;
 std::chrono::steady_clock::time_point started{std::chrono::steady_clock::now()};
 ~Impl(){if(context){[context clearDrawable];[context release];}if(window){[window setDelegate:nil];[window close];[window release];}if(delegate)[delegate release];}
};
Window::Window(int width,int height,const char*title):impl_(std::make_unique<Impl>()){
 @autoreleasepool {
  [NSApplication sharedApplication];[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
  NSString* name=[[NSString alloc] initWithUTF8String:title?title:"POLICE-Stimulator"];
  NSRect rect=NSMakeRect(120,120,width,height);NSUInteger style=NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable;
  impl_->window=[[NSWindow alloc] initWithContentRect:rect styleMask:style backing:NSBackingStoreBuffered defer:NO];
  if(!impl_->window){[name release];throw std::runtime_error("could not create native Cocoa window");}
  [impl_->window setTitle:name];[impl_->window setReleasedWhenClosed:NO];[name release];
  NSOpenGLPixelFormatAttribute attrs[]={NSOpenGLPFAOpenGLProfile,NSOpenGLProfileVersion4_1Core,NSOpenGLPFAColorSize,24,NSOpenGLPFAAlphaSize,8,NSOpenGLPFADepthSize,24,NSOpenGLPFADoubleBuffer,0};
  NSOpenGLPixelFormat* format=[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];if(!format)throw std::runtime_error("OpenGL 4.1 Core pixel format is unavailable");
  impl_->context=[[NSOpenGLContext alloc] initWithFormat:format shareContext:nil];[format release];
  if(!impl_->context)throw std::runtime_error("could not create native NSOpenGLContext");
  PoliceInputView* view=[[PoliceInputView alloc] initWithFrame:rect];view->input_state=&impl_->state;
  [view setWantsBestResolutionOpenGLSurface:YES];[impl_->window setContentView:view];[impl_->window makeFirstResponder:view];[view release];
  impl_->delegate=[[PoliceWindowDelegate alloc] init];impl_->delegate->input_state=&impl_->state;[impl_->window setDelegate:impl_->delegate];
  [impl_->context setView:[impl_->window contentView]];[impl_->context makeCurrentContext];
  GLint swap=1;[impl_->context setValues:&swap forParameter:NSOpenGLContextParameterSwapInterval];
  [impl_->window center];[impl_->window makeKeyAndOrderFront:nil];[NSApp activateIgnoringOtherApps:YES];
 }
 police::core::log(police::core::LogLevel::Info,"Native Cocoa/NSOpenGL platform initialized");
}
Window::~Window()=default;
bool Window::should_close()const{return impl_->state.closing;}
bool Window::key_down(Key key)const{auto i=index(key);return i<impl_->state.keys.size()&&impl_->state.keys[i];}
void Window::poll_events(){@autoreleasepool{for(;;){NSEvent*e=[NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];if(!e)break;[NSApp sendEvent:e];}[impl_->window update];}}
void Window::swap_buffers()const{[impl_->context flushBuffer];}
void Window::request_close(){impl_->state.closing=true;[impl_->window performClose:nil];}
int Window::framebuffer_width()const{const NSRect r=[[impl_->window contentView] convertRectToBacking:[[impl_->window contentView] bounds]];return static_cast<int>(r.size.width);}
int Window::framebuffer_height()const{const NSRect r=[[impl_->window contentView] convertRectToBacking:[[impl_->window contentView] bounds]];return static_cast<int>(r.size.height);}
double Window::time_seconds()const{return std::chrono::duration<double>(std::chrono::steady_clock::now()-impl_->started).count();}
void Window::set_title(const std::string&title){NSString*s=[[NSString alloc] initWithUTF8String:title.c_str()];[impl_->window setTitle:s?s:@"POLICE-Stimulator"];[s release];}
void* Window::get_proc_address(const char*name)const{return dlsym(RTLD_DEFAULT,name);}
} // namespace police::platform

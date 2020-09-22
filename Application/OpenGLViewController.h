#if defined(TARGET_IOS) || defined(TARGET_TVOS)
@import UIKit;
#define PlatformViewBase UIView
#define PlatformViewController UIViewController
#define PlatformGLContext EAGLContext
#else
@import AppKit;
#define PlatformViewBase NSOpenGLView
#define PlatformViewController NSViewController
#define PlatformGLContext NSOpenGLContext
#endif

@interface OpenGLView : PlatformViewBase

@end

@interface OpenGLViewController : PlatformViewController

@end

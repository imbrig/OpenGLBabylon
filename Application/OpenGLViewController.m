#define GL_SILENCE_DEPRECATION
#import <Foundation/Foundation.h>

#import "OpenGLViewController.h"
#import "BabylonView.h"

#import <GLKit/GLKTextureLoader.h>

#if TARGET_MACOS
#import <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#import <OpenGLES/ES3/gl.h>
#endif // !(TARGET_IOS || TARGET_TVOS)

@implementation OpenGLView

#if TARGET_IOS
+ (Class) layerClass
{
  return [CAEAGLLayer class];
}
#endif

@end

@implementation OpenGLViewController
{
  OpenGLView *_view;
  PlatformGLContext *_context;
  GLuint _defaultFrameBuffer;
  GLKTextureInfo* _texInfo;
  
#if defined(TARGET_IOS) || defined(TARGET_TVOS)
  GLuint _colorRenderBuffer;
  GLuint _depthRenderBuffer;
  CAEAGLLayer* _eaglLayer;
  CADisplayLink* _displayLink;
#else
  CVDisplayLinkRef _displayLink;
#endif
  BabylonView *_babylonView;
}

- (void)viewDidLoad
{
  [super viewDidLoad];
  _view = (OpenGLView *)self.view;
  [self prepareView];
  [self makeCurrentContext];
}

#if TARGET_MACOS

- (CGSize)drawableSize
{
  CGSize viewSizePoints = _view.bounds.size;
  CGSize viewSizePixels = [_view convertSizeToBacking:viewSizePoints];
  return viewSizePixels;
}

- (void)makeCurrentContext
{
  [_context makeCurrentContext];
}

static CVReturn OpenGLDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                         const CVTimeStamp* now,
                                         const CVTimeStamp* outputTime,
                                         CVOptionFlags flagsIn,
                                         CVOptionFlags* flagsOut,
                                         void* displayLinkContext)
{
  OpenGLViewController *viewController = (__bridge OpenGLViewController*)displayLinkContext;
  [viewController draw];
  return YES;
}

- (void)draw
{
  CGLLockContext(_context.CGLContextObj);
  [_context makeCurrentContext];
  
  [_babylonView render];
//  [_babylonView drawQuad:_defaultFrameBuffer texName:_texInfo.name];
  
  CGLFlushDrawable(_context.CGLContextObj);
  CGLUnlockContext(_context.CGLContextObj);
}

- (void)prepareView
{
  NSOpenGLPixelFormatAttribute attrs[] =
  {
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
    NSOpenGLPFAColorSize, 24,
    NSOpenGLPFAAlphaSize, 8,
    NSOpenGLPFADepthSize, 24,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAAccelerated ,
    0
  };
  
  NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
  NSAssert(pixelFormat, @"No OpenGL pixel format");
  _context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
  CGLLockContext(_context.CGLContextObj);
  [_context makeCurrentContext];
  CGLUnlockContext(_context.CGLContextObj);

  _view.pixelFormat = pixelFormat;
  _view.openGLContext = _context;
  _view.wantsBestResolutionOpenGLSurface = YES;
  
  // Default FBO is 0 on macOS since it uses a traditional OpenGL pixel format model
  _defaultFrameBuffer = 0;
  CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
  // Set the renderer output callback function
  CVDisplayLinkSetOutputCallback(_displayLink, &OpenGLDisplayLinkCallback, (__bridge void*)self);
  CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, _context.CGLContextObj, pixelFormat.CGLPixelFormatObj);
  
  _texInfo = [self glTextureFromFile];
  _babylonView = [[BabylonView alloc] initWithWidth:1280 height:720];
}

- (void)viewDidLayout
{
  CGLLockContext(_context.CGLContextObj);
  NSSize viewSizePoints = _view.bounds.size;
  NSSize viewSizePixels = [_view convertSizeToBacking:viewSizePoints];
  [self makeCurrentContext];

  // Update size to Babylon
  [_babylonView setSizeWithWidth:viewSizePixels.width height:viewSizePixels.height];

  CGLUnlockContext(_context.CGLContextObj);
  if(!CVDisplayLinkIsRunning(_displayLink))
  {
    CVDisplayLinkStart(_displayLink);
  }
}

- (void)viewWillDisappear
{
  CVDisplayLinkStop(_displayLink);
}

- (void)dealloc
{
  CVDisplayLinkStop(_displayLink);
  CVDisplayLinkRelease(_displayLink);
}

#else

- (void)draw:(id)sender
{
  [EAGLContext setCurrentContext:_context];
  
  glBindFramebuffer(GL_FRAMEBUFFER, _defaultFrameBuffer);
  [_babylonView render];
//  [_babylonView drawQuad:_defaultFrameBuffer texName:_texInfo.name];
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  
  [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)makeCurrentContext
{
  [EAGLContext setCurrentContext:_context];
}

- (void)setupDisplayLink
{
  // Create the display link so that we render at 60FPS
  _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];
  _displayLink.preferredFramesPerSecond = 60;
  // Have the display link run on the default runn loop (and the main thread)
  [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)setupLayer
{
  // Get the layer
  _eaglLayer = (CAEAGLLayer *)self.view.layer;
  _eaglLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking : @NO, kEAGLDrawablePropertyColorFormat : kEAGLColorFormatRGBA8 };
  _eaglLayer.opaque = YES;
}

- (void)setupContext
{
  _context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
  NSAssert(_context, @"Could Not Create OpenGL ES Context");
  BOOL isSetCurrent = [EAGLContext setCurrentContext:_context];
  NSAssert(isSetCurrent, @"Could not make OpenGL ES context current");
}

- (void)setupRenderBuffer
{
//  _defaultFrameBuffer = (GLuint)[_babylonView frameBufferId];
//  _colorRenderBuffer = (GLuint)[_babylonView renderBufferId];
  
  glGenRenderbuffers(1, &_depthRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _view.frame.size.width * 2, _view.frame.size.height * 2);
  
  glGenRenderbuffers(1, &_colorRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  
  // On iOS & tvOS you must create an FBO and attach a CoreAnimation allocated drawable texture to use as the "defaultFBO" for a view
  glGenFramebuffers(1, &_defaultFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, _defaultFrameBuffer);
  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
}

- (void)prepareView
{
  [self setupLayer];
  [self setupContext];
  self.view.contentScaleFactor = [UIScreen mainScreen].nativeScale;
  _texInfo = [self glTextureFromFile];
  
  _babylonView = [[BabylonView alloc] initWithWidth:_view.frame.size.width height:_view.frame.size.height];
  [self setupRenderBuffer];
  [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)_view.layer];
  [self setupDisplayLink];
}

- (CGSize)drawableSize
{
  GLint backingWidth = 0, backingHeight = 0;
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
  CGSize drawableSize = {backingWidth, backingHeight};
  return drawableSize;
}

- (void)resizeDrawable
{
  [self makeCurrentContext];

  CGSize drawSize = [self drawableSize];
  glViewport(0, 0, drawSize.width, drawSize.height);
  
  // Ensure we've actually got a render buffer first;
  assert(_colorRenderBuffer != 0);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  
  // Update size to Babylon
  [_babylonView setSizeWithWidth:drawSize.width height:drawSize.height];
  
//  CGSize resolution = _view.frame.size;
//  [_babylonView setSizeWithWidth:resolution.width height:resolution.height];
  
  [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)_view.layer];
}

- (void)viewDidLayoutSubviews
{
  [self resizeDrawable];
}

- (void)viewDidAppear:(BOOL)animated
{
  [self resizeDrawable];
}

#endif

- (GLKTextureInfo*)glTextureFromFile
{
  NSError *error;
  NSURL *baseTextureURL = [[NSBundle mainBundle] URLForResource:@"Assets/Colors" withExtension:@"png"];
  NSDictionary *option = nil;// @{ GLKTextureLoaderOriginBottomLeft : @YES};
  GLKTextureInfo* texInfo = [GLKTextureLoader textureWithContentsOfURL:baseTextureURL options:option error:&error];
  NSAssert(texInfo, @"Failed to load texture at %@: %@", baseTextureURL.absoluteString, error);
  return texInfo;
}

@end

#define GL_SILENCE_DEPRECATION
#import <Foundation/Foundation.h>

#import "OpenGLViewController.h"
#import "BabylonManager.h"

#if TARGET_MACOS
#import <OpenGL/OpenGL.h>
#import <OpenGL/gl.h>
#import <OpenGL/gl3.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
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

typedef struct {
    float Position[3];
    float Color[4];
//    float TexCoord[2]; // New
} Vertex;

//#define TEX_COORD_MAX   4
//
//const Vertex Vertices[] = {
//    {{1, -1, 0}, {1, 0, 0, 1}, {TEX_COORD_MAX, 0}},
//    {{1, 1, 0}, {0, 1, 0, 1}, {TEX_COORD_MAX, TEX_COORD_MAX}},
//    {{-1, 1, 0}, {0, 0, 1, 1}, {0, TEX_COORD_MAX}},
//    {{-1, -1, 0}, {0, 0, 0, 1}, {0, 0}}
//};

const Vertex Vertices[] = {
  {{1, -1, 0}, {1, 0, 0, 1}},
  {{1, 1, 0}, {0, 1, 0, 1}},
  {{-1, 1, 0}, {0, 0, 1, 1}},
  {{-1, -1, 0}, {0, 0, 0, 1}}
};

const GLubyte Indices[] = {
  0, 1, 2,
  2, 3, 0
};

@implementation OpenGLViewController
{
  OpenGLView *_view;
  PlatformGLContext *_context;
  
  GLuint _defaultFrameBuffer;
#if defined(TARGET_IOS) || defined(TARGET_TVOS)
  CAEAGLLayer* _eaglLayer;
  GLuint _colorRenderBuffer;
//  GLuint _depthRenderBuffer;
  
  GLuint _vertexBuffer;
  GLuint _indexBuffer;
  
  GLuint _positionSlot;
  GLuint _colorSlot;
//  GLuint _texCoordSlot;
  
  CADisplayLink *_displayLink;
#else
  CVDisplayLinkRef _displayLink;
#endif
  BabylonManager *_babylonManager;
}

- (void)viewDidLoad
{
  [super viewDidLoad];
  _view = (OpenGLView *)self.view;
  [self prepareView];
  [self makeCurrentContext];
#if defined(TARGET_IOS) || defined(TARGET_TVOS)
  _babylonManager = [[BabylonManager alloc] initWithWidth:_view.frame.size.width*2 height:_view.frame.size.height*2];
#else
  _babylonManager = [[BabylonManager alloc] initWithWidth:1280 height:720];
#endif
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
  [_babylonManager draw];
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
}

- (void)viewDidLayout
{
  CGLLockContext(_context.CGLContextObj);
  NSSize viewSizePoints = _view.bounds.size;
  NSSize viewSizePixels = [_view convertSizeToBacking:viewSizePoints];
  [self makeCurrentContext];

  // Update size to Babylon
  [_babylonManager setSizeWithWidth:viewSizePixels.width height:viewSizePixels.height];

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
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glClearColor(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glEnable(GL_DEPTH_TEST);
  
//  NSLog(@"ViewSize %f %f", _view.frame.size.width, _view.frame.size.height);
  glViewport(0, 0, _view.frame.size.width, _view.frame.size.height);
  
  
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
  
  glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
  glVertexAttribPointer(_colorSlot, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (sizeof(float) * 3));
  
//  glVertexAttribPointer(_texCoordSlot, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) (sizeof(float) * 7));
  
  glDrawElements(GL_TRIANGLES, sizeof(Indices)/sizeof(Indices[0]), GL_UNSIGNED_BYTE, 0);
  
//  glActiveTexture(GL_TEXTURE0);
//  glBindTexture(GL_TEXTURE_2D, _floorTexture);
//  glUniform1i(_textureUniform, 0);
  
//  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  [_babylonManager draw];
  
  [_context presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)makeCurrentContext
{
  [EAGLContext setCurrentContext:_context];
}

- (void)prepareView
{
  [self setupLayer];
  [self setupContext];
//  [self makeCurrentContext];
  self.view.contentScaleFactor = [UIScreen mainScreen].nativeScale;
  
//  [self setupDepthBuffer];
  [self setupRenderBuffer];
  [self setupFrameBuffer];
  
  // On iOS & tvOS you must create an FBO and attach a CoreAnimation allocated drawable texture
  // to use as the "defaultFBO" for a view
//  glGenFramebuffers(1, &_defaultFBOName);
//  glBindFramebuffer(GL_FRAMEBUFFER, _defaultFBOName);
//  glGenRenderbuffers(1, &_colorRenderBuffer);
//  [self resizeDrawable];
//  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);

  [self compileShaders];
  [self setupVBO];
  
  [self setupDisplayLink];
}

- (CGSize)drawableSize
{
  GLint backingWidth, backingHeight;
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
  glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
  CGSize drawableSize = {backingWidth, backingHeight};
  return drawableSize;
}

- (void)setupDisplayLink
{
  // Create the display link so that we render at 60FPS
  _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(draw:)];
  _displayLink.preferredFramesPerSecond = 60;
  // Have the display link run on the default runn loop (and the main thread)
  [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)resizeDrawable
{
//  [self makeCurrentContext];

  // Ensure we've actually got a render buffer first;
  assert(_colorRenderBuffer != 0);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
  [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)_view.layer];

  // Update size to Babylon
  CGSize resolution = _view.frame.size;//[self drawableSize];
  [_babylonManager setSizeWithWidth:resolution.width*2 height:resolution.height*2];
}

- (void)setupLayer
{
  // Get the layer
  _eaglLayer = (CAEAGLLayer *)self.view.layer;
  _eaglLayer.drawableProperties = @{kEAGLDrawablePropertyRetainedBacking : @NO,
                                   kEAGLDrawablePropertyColorFormat     : kEAGLColorFormatRGBA8 };
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
  glGenRenderbuffers(1, &_colorRenderBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
//  [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}

//- (void)setupDepthBuffer
//{
//  CGSize resolution = {750, 1296};//[self drawableSize];
//  glGenRenderbuffers(1, &_depthRenderBuffer);
//  glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
//  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, resolution.width, resolution.height);
//}

- (void)setupFrameBuffer
{
  _defaultFrameBuffer = 0;
  glGenFramebuffers(1, &_defaultFrameBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, _defaultFrameBuffer);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
//  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
}

- (GLuint)compileShader:(NSString*)shaderString withType:(GLenum)shaderType
{
    GLuint shaderHandle = glCreateShader(shaderType);
    const char * shaderStringUTF8 = [shaderString UTF8String];
    int shaderStringLength = (int)[shaderString length];
    glShaderSource(shaderHandle, 1, &shaderStringUTF8, &shaderStringLength);
    glCompileShader(shaderHandle);
    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
    if(compileSuccess == GL_FALSE)
    {
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle, sizeof(messages), 0, &messages[0]);
        NSString *messageString = [NSString stringWithUTF8String:messages];
        NSLog(@"%@", messageString);
        exit(1);
    }
    return shaderHandle;
}

- (void)compileShaders
{
  NSString * vertexString =  @"attribute vec4 Position;\n"
                              "attribute vec4 SourceColor;\n"
                              "varying vec4 DestinationColor;\n"
                              "void main(void) {\n"
                              "  DestinationColor = SourceColor;\n"
                              "  gl_Position = Position; }\n";

  NSString * fragmentString =  @"varying lowp vec4 DestinationColor;\n"
                                "void main(void) {;\n"
                                "  gl_FragColor = DestinationColor; }\n";
  
  GLuint vertexShader = [self compileShader:vertexString withType:GL_VERTEX_SHADER];
  GLuint fragmentShader = [self compileShader:fragmentString withType:GL_FRAGMENT_SHADER];
  
  GLuint programHandle = glCreateProgram();
  glAttachShader(programHandle, vertexShader);
  glAttachShader(programHandle, fragmentShader);
  glLinkProgram(programHandle);
  GLint linkSuccess;
  glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
  if(linkSuccess == GL_FALSE)
  {
    GLchar messages[256];
    glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
    NSString *messageString = [NSString stringWithUTF8String:messages];
    NSLog(@"%@", messageString);
    exit(1);
  }
  
  glUseProgram(programHandle);
  _positionSlot = glGetAttribLocation(programHandle, "Position");
  _colorSlot = glGetAttribLocation(programHandle, "SourceColor");
  glEnableVertexAttribArray(_positionSlot);
  glEnableVertexAttribArray(_colorSlot);
}

- (void)setupVBO
{
  glGenBuffers(1, &_vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
  
  glGenBuffers(1, &_indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
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


@end

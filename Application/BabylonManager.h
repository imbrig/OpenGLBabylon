#pragma once

#import <Foundation/Foundation.h>

#if TARGET_MACOS
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/gl.h>
#endif // !(TARGET_IOS || TARGET_TVOS)

@interface BabylonManager : NSObject

- (instancetype)initWithWidth:(int)width height:(int)height;
- (void)setSizeWithWidth:(int)width height:(int)height;
- (void)draw;
- (void)drawQuad:(GLuint)defaultFrameBuffer texTarget:(GLenum)texTarget texName:(GLuint)texName;
- (void)update;

- (unsigned int)frameBufferId;
- (unsigned int)renderBufferId;

@end

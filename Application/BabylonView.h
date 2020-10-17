#pragma once

#import <Foundation/Foundation.h>

#if TARGET_MACOS
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#include <OpenGLES/ES3/gl.h>
#endif // !(TARGET_IOS || TARGET_TVOS)

@interface BabylonView : NSObject

- (instancetype)initWithWidth:(int)width height:(int)height;
- (void)setSizeWithWidth:(int)width height:(int)height;
- (void)render;
- (void)update;

- (unsigned int)frameBufferId;
- (unsigned int)renderBufferId;
- (unsigned int)textureBufferId;

@end

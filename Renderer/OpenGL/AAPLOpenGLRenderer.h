/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for renderer class which performs OpenGL state setup and per frame rendering
*/

#import <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include "AAPLGLHeaders.h"
#import <GLKit/GLKTextureLoader.h>

static const CGSize AAPLInteropTextureSize = {800, 600};

@interface AAPLOpenGLRenderer : NSObject

- (instancetype)initWithDefaultFBOName:(GLuint)defaultFBOName;
- (void)draw;
- (void)resize:(CGSize)size;
- (void)useTextureFromFileAsBaseMap;

@end

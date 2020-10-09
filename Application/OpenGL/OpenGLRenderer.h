#import <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include "GLHeaders.h"

@interface OpenGLRenderer : NSObject

- (instancetype)init;
- (void)draw:(GLuint)defaultFrameBuffer texTarget:(GLenum)texTarget texName:(GLuint)texName;
- (void)resize:(CGSize)size;

@end

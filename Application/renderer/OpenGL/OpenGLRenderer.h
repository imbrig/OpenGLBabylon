#import <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include "GLHeaders.h"

@interface OpenGLRenderer : NSObject

- (instancetype)init;
- (void)draw:(GLuint)defaultFrameBuffer;
- (void)resize:(CGSize)size;
- (void)useTextureFromFileAsBaseMap;

@end

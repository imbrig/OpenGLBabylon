#import <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include "GLHeaders.h"

@interface OpenGLRenderer : NSObject

- (instancetype)initWithDefaultFBOName:(GLuint)defaultFBOName;
- (void)draw;
- (void)resize:(CGSize)size;
- (void)useTextureFromFileAsBaseMap;

@end

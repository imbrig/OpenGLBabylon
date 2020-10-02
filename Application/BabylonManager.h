#pragma once

#import <Foundation/Foundation.h>

@interface BabylonManager : NSObject

- (instancetype)initWithWidth:(int)width height:(int)height;
- (void)setSizeWithWidth:(int)width height:(int)height;
- (void)draw;
- (void)update;

@end

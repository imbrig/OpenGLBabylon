#pragma once

#import <Foundation/Foundation.h>

@interface BabylonManager : NSObject

- (instancetype)init;
- (void)initializeWithWidth:(int)width height:(int)height;
- (void)draw;
- (void)update;

@end

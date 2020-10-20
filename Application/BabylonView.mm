#import "BabylonView.h"
#import "interface/BabylonManager.h"
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>

@interface BabylonView()
@property (nonatomic, readonly) Interface::BabylonManager* launcher;
@end

@implementation BabylonView

- (instancetype)initWithWidth:(int)width height:(int)height
{
  if(self = [super init])
  {
    _launcher = new Interface::BabylonManager(width, height);
  }
  return self;
}

- (void)dealloc
{
  delete _launcher;
}

- (void)setSizeWithWidth:(int)width height:(int)height
{
  _launcher->setSize(width, height);
}

- (void)render
{
  _launcher->render();
//  [self saveRenderBufferToImage];
}

- (void)update
{
}

- (unsigned int)frameBufferId
{
  return _launcher->frameBufferId();
}

- (unsigned int)renderBufferId
{
  return _launcher->renderBufferId();
}

- (unsigned int)textureBufferId
{
  return _launcher->textureBufferId();
}

- (void)saveRenderBufferToImage
{
  GLubyte* pixelData = _launcher->getPixelDataFromFrameBuffer();
  int width = _launcher->_width;
  int height = _launcher->_height;
  int dataLength = width * height * 4;
  // Set up for CGImage creation
  CGDataProviderRef providerRef = CGDataProviderCreateWithData(NULL, pixelData, dataLength, NULL);
  CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
  CGImageRef imageRef = CGImageCreate(width, height, 8, 32, width * 4, colorspace, kCGImageAlphaNoneSkipLast, providerRef, NULL, true, kCGRenderingIntentDefault);
  
  UIGraphicsBeginImageContext(CGSizeMake(width, height));
  CGContextRef cgcontext = UIGraphicsGetCurrentContext();
  CGContextSetBlendMode(cgcontext, kCGBlendModeCopy);
  // Sometimes we get a padding number that we must crop out
  CGContextDrawImage(cgcontext, CGRectMake(0.0, 0.0, width, height), imageRef);
  UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
  UIGraphicsEndImageContext();
  
  // Free up objects
  free(pixelData);
  CFRelease(providerRef);
  CFRelease(colorspace);
  CGImageRelease(imageRef);
  
  NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
  NSString *filePath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"BabylonRenderResult.png"];

  // Save image.
  [UIImagePNGRepresentation(image) writeToFile:filePath atomically:YES];
}
@end

#import "BabylonManager.h"
#import "interface/BabylonLauncher.h"

@interface BabylonManager()
@property (nonatomic, readonly) Launcher::BabylonLauncher* launcher;
@end

@implementation BabylonManager

- (instancetype)initWithWidth:(int)width height:(int)height
{
  if(self = [super init])
  {
    _launcher = new Launcher::BabylonLauncher(width, height);
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

- (void)draw
{
  _launcher->run();
}

- (void)drawQuad:(GLuint)defaultFrameBuffer texName:(GLuint)texName
{
  _launcher->drawQuad(defaultFrameBuffer, texName);
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

@end

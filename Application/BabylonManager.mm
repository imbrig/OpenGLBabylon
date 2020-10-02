#import "BabylonManager.h"
#import "interface/BabylonLauncher.h"

@interface BabylonManager()
@property (nonatomic, readonly) Launcher::BabylonLauncher* launcher;
@end

@implementation BabylonManager

- (instancetype)init
{
  if(self = [super init])
  {
    int width = 1280;
    int height = 720;
    _launcher = new Launcher::BabylonLauncher(width, height);
  }
  return self;
}

- (void)dealloc
{
  delete _launcher;
}

- (void)initializeWithWidth:(int)width height:(int)height
{
//  _launcher->intialize(width, height);
}

- (void)draw
{
  _launcher->run();
}

- (void)update
{
//  _launcher->update();
}

@end

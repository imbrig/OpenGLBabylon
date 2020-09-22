#import "BabylonManager.h"
#import "interface/BabylonLauncher.h"

@interface BabylonManager()
@property (nonatomic, readonly) BabylonLauncher* launcher;
@end

@implementation BabylonManager

- (instancetype)init
{
  if(self = [super init])
  {
    _launcher = new BabylonLauncher();
  }
  return self;
}

- (void)dealloc
{
  delete _launcher;
}

- (void)draw
{
  _launcher->draw();
}

- (void)update
{
  _launcher->update();
}

@end

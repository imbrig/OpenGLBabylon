#import "BabylonView.h"
#import "interface/BabylonManager.h"

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
}

- (void)update
{
}

@end

#ifndef BABYLONLAUNCHER_H
#define BABYLONLAUNCHER_H

#include <memory>

namespace BABYLON {
class ICanvas;
class IRenderableScene;
}

namespace Launcher {

class BabylonLauncher {
public:
  int _width                = 0;
  int _height               = 0;
  bool _intialized          = false;
  
  BabylonLauncher(int width, int height);
  ~BabylonLauncher();
  
  void run();
  void setSize(int width, int height);
  
private:
  // Babylon scene related variables
  std::unique_ptr<BABYLON::ICanvas> _renderCanvas;
  std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
}; // end of class SimpleLauncher

} // end of namespace Launcher

#endif

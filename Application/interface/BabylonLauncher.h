#ifndef BABYLONLAUNCHER_H
#define BABYLONLAUNCHER_H

#include <memory>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>

class BabylonLauncher {
  std::shared_ptr<BABYLON::IRenderableScene> _scene;
  std::unique_ptr<BABYLON::ICanvas> _renderCanvas;
  
public:
  BabylonLauncher();
  ~BabylonLauncher();
  
  void intialize(int width, int height);
  void draw();
  void update();
  
  BABYLON::ICanvas* getRenderCanvas();
};

#endif

#ifndef BabylonManager_H
#define BabylonManager_H

#include "framebuffer_canvas.h"
//#include "basic_canvas.h"
#include <memory>

#if TARGET_MACOS
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#include <OpenGLES/ES3/gl.h>
#endif // !(TARGET_IOS || TARGET_TVOS)

namespace BABYLON {
class ICanvas;
class IRenderableScene;
}

namespace Interface {

class BabylonManager {
public:
  int _width = 0;
  int _height = 0;
  bool _intialized = false;
  
  BabylonManager(int width, int height);
  ~BabylonManager();
  
  void render();
  void setSize(int width, int height);
  
  unsigned int frameBufferId();
  unsigned int renderBufferId();
  unsigned int textureBufferId();
  
private:
  // Babylon scene related variables
//  std::unique_ptr<BasicCanvas> _renderCanvas;
  std::unique_ptr<FrameBufferCanvas> _renderCanvas;
  std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
}; // end of class SimpleLauncher

} // end of namespace Interface

#endif

#include "BabylonLauncher.h"
#include "MakeScene.h"

// Babylon
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

namespace Launcher {

BabylonLauncher::BabylonLauncher(int width, int height):
  _width(width),
  _height(height),
  _renderableScene(nullptr)
{
  _renderCanvas = std::make_unique<BABYLON::impl::FrameBufferCanvas>();
  _renderCanvas->clientWidth = _width;
  _renderCanvas->clientHeight = _height;
  
  // Initialize 3D context
//  if(!_renderCanvas->initializeContext3d())
//  {
//    fprintf(stderr, "Error occured, Failed to initialize 3D context\n");
//    return;
//  }
  
  _renderCanvas->setFrameSize(_width, _height);
  _renderCanvas->initializeFrameBuffer();
  
  _renderableScene = Samples::MakeHelloScene(_renderCanvas.get());
//  _renderableScene = Samples::MakeStarfieldProceduralTextureScene(_renderCanvas.get());
//  _renderableScene = Samples::MakeShaderMaterialDoubleHelixScene(_renderCanvas.get());
  
  _renderableScene->initialize(_renderCanvas.get());
  _intialized = true;
}

BabylonLauncher::~BabylonLauncher()
{
}

void BabylonLauncher::run()
{
  // Render Scene
  _renderCanvas->bind();
  _renderableScene->render();
  _renderCanvas->unbind();
  
  unsigned int id = _renderCanvas->textureId();
}

void BabylonLauncher::setSize(int width, int height)
{
  if(_width != width || _height != height)
  {
    _width = width;
    _height = height;
    _renderCanvas->setFrameSize(_width, _height);
    _renderableScene->getEngine()->resize();
  }
}

} // end of namespace Launcher

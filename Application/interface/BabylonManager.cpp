#include "BabylonManager.h"
#include "MakeScene.h"

// Babylon
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

#include <stdio.h>
#include <assert.h>

namespace Interface {

BabylonManager::BabylonManager(int width, int height):
  _width(width),
  _height(height),
  _renderableScene(nullptr)
{
//  _renderCanvas = std::make_unique<BABYLON::impl::FrameBufferCanvas>();
  _renderCanvas = std::make_unique<BasicCanvas>();
  _renderCanvas->clientWidth = _width;
  _renderCanvas->clientHeight = _height;
  
  _renderCanvas->setFrameSize(_width, _height);
  
  int pickScene = 1;
  switch(pickScene)
  {
    case 1:
      _renderableScene = Samples::MakeHelloScene(_renderCanvas.get());
      break;
    case 2:
      _renderableScene = Samples::MakeStarfieldProceduralTextureScene(_renderCanvas.get());
      break;
    case 3:
      _renderableScene = Samples::MakeShaderMaterialDoubleHelixScene(_renderCanvas.get());
      break;
    default:
      _renderableScene = Samples::MakeHelloScene(_renderCanvas.get());
      break;
  }
  
  _renderableScene->initialize(_renderCanvas.get());
  _intialized = true;
}

BabylonManager::~BabylonManager()
{
}

void BabylonManager::render()
{
  // Render Scene
//  _renderCanvas->bind();
  _renderableScene->render();
//  _renderCanvas->unbind();
}

void BabylonManager::setSize(int width, int height)
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

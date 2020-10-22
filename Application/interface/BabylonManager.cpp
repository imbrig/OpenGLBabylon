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
  _renderCanvas = std::make_unique<FrameBufferCanvas>();
//  _renderCanvas = std::make_unique<BasicCanvas>();
  _renderCanvas->clientWidth = _width;
  _renderCanvas->clientHeight = _height;
  _renderCanvas->setFrameSize(_width, _height);
  _renderCanvas->initializeFrameBuffer();
  
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
  _renderCanvas->bind();
//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  _renderableScene->render();
  
  _renderCanvas->unbind();
}

GLubyte* BabylonManager::getPixelDataFromFrameBuffer()
{
  _renderCanvas->bind();
  const int width = _renderCanvas->clientWidth;
  const int height = _renderCanvas->clientHeight;
    
  // Grabbing image from FBO
  //glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    
  // Call draw function so that rendering both presented
  //glViewport(0, 0, width, height);
    
  glFinish();
    
  int dataLength = width * height * 4;
  GLubyte *data = (GLubyte*)malloc(dataLength * sizeof(GLubyte));
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  _renderCanvas->unbind();
  
  return data;
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

unsigned int BabylonManager::frameBufferId()
{
  return _renderCanvas->frameBufferId();
}

unsigned int BabylonManager::renderBufferId()
{
  return _renderCanvas->renderBufferId();
}

unsigned int BabylonManager::textureBufferId()
{
  return _renderCanvas->textureBufferId();
}

} // end of namespace Launcher

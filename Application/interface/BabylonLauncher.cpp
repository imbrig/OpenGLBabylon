#include "BabylonLauncher.h"
#include "HelloScene.h"

#include <babylon/babylon_stl_util.h>
#include <babylon/core/logging.h>
#include <babylon/core/time.h>
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/abstract_mesh.h>
#include "canvas.h"

#if TARGET_MACOS
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/gl.h>
#endif // !(TARGET_IOS || TARGET_TVOS)


BabylonLauncher::BabylonLauncher()
{
  _renderCanvas = std::make_unique<BABYLON::impl::Canvas>();
  _scene = MakeHelloScene();
  intialize();
}

BabylonLauncher::~BabylonLauncher()
{
}

void BabylonLauncher::intialize()
{
  int width = 800;
  int height = 600;
  _renderCanvas->initializeContext3d();
  _renderCanvas->setFrameSize(width, height);
  // Initialize Canvas before Scene
  _scene->initialize(_renderCanvas.get());
}

void BabylonLauncher::draw()
{
  // Make current context should be done before calling draw, from the app level.
  glClearColor(0.f, 0.f, 1.f, 1.f);
  // Render Scene
  if(_scene)
  {
    _scene->render();
  }
  // Call swap front and back buffers after calling draw, at the app level.
}

void BabylonLauncher::update()
{
  // Call update scene logic here
}

BABYLON::ICanvas* BabylonLauncher::getRenderCanvas()
{
  return _renderCanvas.get();
}

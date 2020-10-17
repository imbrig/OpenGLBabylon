#ifndef BABYLON_IMPL_FRAME_BUFFER_CANVAS_H
#define BABYLON_IMPL_FRAME_BUFFER_CANVAS_H

#include "gl_rendering_context.h"
#include <babylon/babylon_api.h>
#include <babylon/interfaces/icanvas.h>

namespace Interface {

class FrameBufferCanvas : public BABYLON::ICanvas
{
public:
  FrameBufferCanvas();
  ~FrameBufferCanvas() override;
  void initializeFrameBuffer();
  void resize(int _width, int _height);

  BABYLON::ClientRect& getBoundingClientRect() override;
  bool initializeContext3d() override;
  BABYLON::ICanvasRenderingContext2D* getContext2d() override;
  BABYLON::GL::IGLRenderingContext* getContext3d(const BABYLON::EngineOptions& options) override;

  void bind();
  void unbind();
  
  void renderBufferBind();
  void renderBufferUnbind();
  
  unsigned int frameBufferId();
  unsigned int textureBufferId();
  unsigned int renderBufferId();

private:
  std::shared_ptr<BABYLON::GL::IGLFramebuffer> mFrameBuffer;
  std::shared_ptr<BABYLON::GL::IGLRenderbuffer> mRenderbuffer;
  std::shared_ptr<BABYLON::GL::IGLTexture> mTextureColorBuffer;
};

} // end of namespace Interface

#endif // BABYLON_IMPL_FRAME_BUFFER_CANVAS_H

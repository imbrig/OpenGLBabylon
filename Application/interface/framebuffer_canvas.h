#ifndef BABYLON_IMPL_FRAME_BUFFER_CANVAS_H
#define BABYLON_IMPL_FRAME_BUFFER_CANVAS_H

#include "gl_rendering_context.h"
#include <babylon/babylon_api.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {
namespace impl {

class FrameBufferCanvas : public ICanvas
{
public:
  FrameBufferCanvas();
  ~FrameBufferCanvas() override;
  void initializeFrameBuffer();
  void resize(int _width, int _height);

  ClientRect& getBoundingClientRect() override;
  bool initializeContext3d() override;
  ICanvasRenderingContext2D* getContext2d() override;
  GL::IGLRenderingContext* getContext3d(const EngineOptions& options) override;

  void bind();
  void unbind();

  unsigned int frameBufferId();
  unsigned int colorBufferId();
  unsigned int renderBufferId();

private:
  std::shared_ptr<BABYLON::GL::IGLFramebuffer> mFrameBuffer;
  std::shared_ptr<BABYLON::GL::IGLTexture> mTextureColorBuffer;
  std::shared_ptr<GL::IGLRenderbuffer> mRenderbuffer;
};

} // end of namespace impl
} // end of namespace BABYLON

#endif // BABYLON_IMPL_FRAME_BUFFER_CANVAS_H

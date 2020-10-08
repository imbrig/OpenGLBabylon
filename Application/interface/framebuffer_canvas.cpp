#include "framebuffer_canvas.h"
#include "gl_rendering_context.h"

#if TARGET_MACOS
#include <OpenGL/gl.h>
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#else // if (TARGET_IOS || TARGET_TVOS)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/ES3/gl.h>
#endif // !(TARGET_IOS || TARGET_TVOS)

namespace BABYLON {
namespace impl {

FrameBufferCanvas::FrameBufferCanvas() :
  mFrameBuffer{nullptr},
  mTextureColorBuffer{nullptr},
  mRenderbuffer{nullptr}
{
  _renderingContext = std::make_unique<GL::GLRenderingContext>();
  _renderingContext->initialize();
  _boundingClientRect.bottom = clientHeight;
  _boundingClientRect.height = clientHeight;
  _boundingClientRect.left   = 0;
  _boundingClientRect.right  = clientWidth;
  _boundingClientRect.top    = 0;
  _boundingClientRect.width  = clientWidth;
}

FrameBufferCanvas::~FrameBufferCanvas()
{
  _renderingContext->deleteFramebuffer(mFrameBuffer.get());
  _renderingContext->deleteTexture(mTextureColorBuffer.get());
  _renderingContext->deleteRenderbuffer(mRenderbuffer.get());
}

void FrameBufferCanvas::initializeFrameBuffer()
{
  _renderingContext->drawingBufferWidth  = clientWidth;
  _renderingContext->drawingBufferHeight = clientHeight;

  // Create a frame buffer
  mFrameBuffer = _renderingContext->createFramebuffer();
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());

  // Create a color attachment texture
  mTextureColorBuffer = _renderingContext->createTexture();
  _renderingContext->bindTexture(GL_TEXTURE_2D, mTextureColorBuffer.get());
  _renderingContext->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, clientWidth, clientHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  _renderingContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  _renderingContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Create a renderbuffer object for depth and stencil attachment
  mRenderbuffer = _renderingContext->createRenderbuffer();
  _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer.get());
  _renderingContext->renderbufferStorage(GL_RENDERBUFFER, GL::DEPTH24_STENCIL8, clientWidth, clientHeight);
  _renderingContext->framebufferRenderbuffer(GL_FRAMEBUFFER, GL::DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderbuffer.get());

  // Set "renderedTexture" as our colour attachement #0
  _renderingContext->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBuffer.get(), 0);

  // Set the list of draw buffers.
  std::vector<GL::GLenum> drawBuffers{GL_COLOR_ATTACHMENT0};
  _renderingContext->drawBuffers(drawBuffers); // "1" is the size of DrawBuffers

  if (_renderingContext->checkFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
  }

  _renderingContext->bindTexture(GL_TEXTURE_2D, nullptr);
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, nullptr);
}

void FrameBufferCanvas::resize(int iWidth, int iHeight)
{
  if(clientWidth != iWidth || clientHeight != iHeight)
  {
    clientWidth                            = iWidth;
    clientHeight                           = iHeight;
    _renderingContext->drawingBufferWidth  = iWidth;
    _renderingContext->drawingBufferHeight = iHeight;

    // Resize texture
    _renderingContext->activeTexture(GL_TEXTURE0);
    _renderingContext->bindTexture(GL_TEXTURE_2D, mTextureColorBuffer.get());
    _renderingContext->texImage2D(GL_TEXTURE_2D, 0, GL_RGB, clientWidth, clientHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Bind the frame buffer
    _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());

    // Resize renderbuffer
    _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, mRenderbuffer.get());
    _renderingContext->renderbufferStorage(GL_RENDERBUFFER, GL::DEPTH24_STENCIL8, clientWidth, clientHeight);

    // Set "renderedTexture" as our colour attachement #0
    _renderingContext->framebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureColorBuffer.get(), 0);

    // Unbind the buffers and texture
    _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, nullptr);
    _renderingContext->bindRenderbuffer(GL_RENDERBUFFER, nullptr);
    _renderingContext->bindTexture(GL_TEXTURE_2D, nullptr);
  }
}

bool FrameBufferCanvas::initializeContext3d()
{
  if (!_initialized) {
    _initialized = _renderingContext->initialize();
  }

  return _initialized;
}

ClientRect& FrameBufferCanvas::getBoundingClientRect()
{
  return _boundingClientRect;
}

ICanvasRenderingContext2D* FrameBufferCanvas::getContext2d()
{
  return nullptr;
}

GL::IGLRenderingContext* FrameBufferCanvas::getContext3d(const EngineOptions& /*options*/)
{
  return _renderingContext.get();
}

void FrameBufferCanvas::bind()
{
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer.get());
}

void FrameBufferCanvas::unbind()
{
  _renderingContext->bindFramebuffer(GL_FRAMEBUFFER, nullptr);
}

unsigned int FrameBufferCanvas::textureId()
{
  return mTextureColorBuffer->value;
}

} // end of namespace GL
} // end of namespace BABYLON

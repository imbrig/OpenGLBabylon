#ifndef BABYLONLAUNCHER_H
#define BABYLONLAUNCHER_H

#include "framebuffer_canvas.h"
#include <memory>

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
  
  unsigned int frameBufferId();
  unsigned int renderBufferId();
  
  void drawQuad(GLuint defaultFrameBuffer, GLenum texTarget, GLuint texName);
  
private:
  // Babylon scene related variables
  std::unique_ptr<BABYLON::impl::FrameBufferCanvas> _renderCanvas;
  std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
  
  GLuint _programName;
  GLuint _vaoName;
  GLenum _baseMapTexTarget;
  GLuint _baseMapTexName;
  
  void resizeQuad(int width, int height);
  GLuint buildQuadVAO();
  void destroyQuadVAO();
  GLuint buildQuadProgramShaders();
}; // end of class SimpleLauncher

// Indicies to which we will set vertex array attibutes
// See buildVAO and buildProgram
enum VertAttrib { POS_ATTRIB_IDX, TEXCOORD_ATTRIB_IDX };

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static inline const char * GetGLErrorString(GLenum error)
{
  const char *str;
  switch( error )
  {
    case GL_NO_ERROR:
      str = "GL_NO_ERROR";
      break;
    case GL_INVALID_ENUM:
      str = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      str = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      str = "GL_INVALID_OPERATION";
      break;
    case GL_OUT_OF_MEMORY:
      str = "GL_OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      str = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
#if defined __gl_h_
    case GL_STACK_OVERFLOW:
      str = "GL_STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      str = "GL_STACK_UNDERFLOW";
      break;
    case GL_TABLE_TOO_LARGE:
      str = "GL_TABLE_TOO_LARGE";
      break;
#endif
    default:
      str = "(ERROR: Unknown Error Enum)";
      break;
  }
  return str;
}

static inline void GetCppGLError(const char * file, int line)
{
  GLenum err = glGetError();
  while(err != GL_NO_ERROR)
  {
    printf("GLError %s set in File:%s Line:%d\n", GetGLErrorString(err), file, line);
    err = glGetError();
  }
}

} // end of namespace Launcher

#endif

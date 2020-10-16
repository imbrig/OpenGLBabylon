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
  
  void drawQuad(GLuint defaultFrameBuffer, GLuint texName);
  
private:
  // Babylon scene related variables
//  std::unique_ptr<BasicCanvas> _renderCanvas;
  std::unique_ptr<FrameBufferCanvas> _renderCanvas;
  std::shared_ptr<BABYLON::IRenderableScene> _renderableScene;
  
  GLuint _programName;
  GLuint _vaoName;
  GLuint _vaoName0;
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

} // end of namespace Interface

#endif
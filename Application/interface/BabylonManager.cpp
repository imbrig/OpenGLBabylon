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
  _vaoName = buildQuadVAO();
  _programName = buildQuadProgramShaders();
  
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
  destroyQuadVAO();
}

void BabylonManager::render()
{
  // Render Scene
  _renderCanvas->bind();
//  glClearColor(0.5, 0, 0, 1);
//  glClear(GL_COLOR_BUFFER_BIT);
//  glEnable(GL_DEPTH_TEST);
  _renderableScene->render();
  _renderCanvas->unbind();
  
  GetCppGLError(__FILE__, __LINE__);
  
//  drawQuad(0, 0);
}

void BabylonManager::setSize(int width, int height)
{
  if(_width != width || _height != height)
  {
    _width = width;
    _height = height;
    resizeQuad(_width, _height);
    
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

void BabylonManager::drawQuad(GLuint defaultFrameBuffer, GLuint texName)
{
//  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_DEPTH_TEST);
  GLuint renderTexture = _renderCanvas->textureBufferId();

  glClearColor(0, 0, 0.5, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_programName);
  glBindVertexArray(_vaoName);
//  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderTexture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
//  glDisable(GL_DEPTH_TEST);
  GetCppGLError(__FILE__, __LINE__);
}

void BabylonManager::resizeQuad(int width, int height)
{
//  glViewport(0, 0, width, height);
//  float aspect = (float)width / (float)height;
}

GLuint BabylonManager::buildQuadVAO()
{
  typedef struct
  {
    float Position[4];
    float TexCoord[2];
  } Vertex;

  static const Vertex QuadVertices[] = {
    //  Positions                    TexCoords
    { { -1.0,  -1.0,  0.0,  1.0 }, { 0.0, 1.0 } },
    { { -1.0,   1.0,  0.0,  1.0 }, { 0.0, 0.0 } },
    { {  1.0,  -1.0,  0.0,  1.0 }, { 1.0, 1.0 } },
    { {  1.0,  -1.0,  0.0,  1.0 }, { 1.0, 1.0 } },
    { { -1.0,   1.0,  0.0,  1.0 }, { 0.0, 0.0 } },
    { {  1.0,   1.0,  0.0,  1.0 }, { 1.0, 0.0 } },
  };

  GLuint vaoName;
  glGenVertexArrays(1, &vaoName);
  glBindVertexArray(vaoName);
  GLuint bufferName;
  glGenBuffers(1, &bufferName);
  glBindBuffer(GL_ARRAY_BUFFER, bufferName);
  glBufferData(GL_ARRAY_BUFFER,  sizeof(QuadVertices), QuadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(POS_ATTRIB_IDX);
  GLuint stride = sizeof(Vertex);
  GLuint positionOffset = offsetof(Vertex, Position);
  glVertexAttribPointer(POS_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(positionOffset));

  // Enable the position attribute for this VAO
  glEnableVertexAttribArray(TEXCOORD_ATTRIB_IDX);
  GLuint texCoordOffset = offsetof(Vertex, TexCoord);
  glVertexAttribPointer(TEXCOORD_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(texCoordOffset));
  GetCppGLError(__FILE__, __LINE__);
  return vaoName;
}

void BabylonManager::destroyQuadVAO()
{
  GLuint index;
  GLuint bufName;
  // Bind the VAO so we can get data from it
  glBindVertexArray(_vaoName);
  // For every possible attribute set in the VAO, delete the attached buffer
  for(index = 0; index < 16; index++)
  {
    glGetVertexAttribiv(index , GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLint*)&bufName);
    if(bufName)
    {
      glDeleteBuffers(1, &bufName);
    }
  }
  glDeleteVertexArrays(1, &_vaoName);
  GetCppGLError(__FILE__, __LINE__);
}

GLuint BabylonManager::buildQuadProgramShaders()
{
  const GLchar* vertexString[] =
  {
#if TARGET_IOS
    "#version 300 es\n",
#else
    "#version 330 core\n",
#endif
    "#ifdef GL_ES\n",
    "precision highp float;\n",
    "#endif\n",
    "#if __VERSION__ >= 140\n",
    "in vec4 inPosition;\n",
    "in vec2 inTexcoord;\n",
    "out vec2 varTexcoord;\n",
    "#else\n",
    "attribute vec4 inPosition;\n",
    "attribute vec2 inTexcoord;\n",
    "varying vec2 varTexcoord;\n",
    "#endif\n",
    "void main (void)\n",
    "{\n",
    "    gl_Position = inPosition;\n",
    "    varTexcoord = inTexcoord;\n",
    "}"
  };

  const GLchar* fragmentString[] =
  {
#if TARGET_IOS
    "#version 300 es\n",
#else
    "#version 330\n",
#endif
    "#ifdef GL_ES\n",
    "precision highp float;\n",
    "#endif\n",
    "#if __VERSION__ >= 140\n",
    "in vec2 varTexcoord;\n",
    "out vec4 fragColor;\n",
    "#else\n",
    "varying vec2 varTexcoord;\n",
    "#endif\n",
    "uniform sampler2D textureMap;\n",
    "void main (void)\n",
    "{\n",
    "    #if __VERSION__ >= 140\n",
    "    fragColor = texture(textureMap, varTexcoord.st, 0.0);\n",
    "    #else\n",
    "    gl_FragColor = texture2D(textureMap, varTexcoord.st, 0.0);\n",
    "    #endif\n",
    "}"
  };

  GLuint prgName;
  GLint logLength, status;

  // String to pass to glShaderSource
  GLchar* sourceString = NULL;

  // Determine if GLSL version 140 is supported by this context.
  // We'll use this info to generate a GLSL shader source string with the proper version preprocessor string prepended
  float glLanguageVersion;

#if TARGET_IOS
  sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "OpenGL ES GLSL ES %f", &glLanguageVersion);
#else
  sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glLanguageVersion);
#endif

  // GL_SHADING_LANGUAGE_VERSION returns the version standard version form
  //  with decimals, but the GLSL version preprocessor directive simply
  //  uses integers (thus 1.10 should 110 and 1.40 should be 140, etc.)
  //  We multiply the floating point number by 100 to get a proper
  //  number for the GLSL preprocessor directive
  GLuint version = 100 * glLanguageVersion;

  // Get the size of the version preprocessor string info so we know how much memory to allocate for our sourceString
  const GLsizei versionStringSize = sizeof("#version 123\n");

  // Create a program object
  prgName = glCreateProgram();

  glBindAttribLocation(prgName, POS_ATTRIB_IDX, "inPosition");
  glBindAttribLocation(prgName, TEXCOORD_ATTRIB_IDX, "inTexcoord");

  //////////////////////////////////////
  // Specify and compile VertexShader //
  //////////////////////////////////////

  int vertStringSize = sizeof(vertexString) / sizeof(*vertexString);
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, vertStringSize, vertexString, NULL);
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);

  if(logLength > 0)
  {
    GLchar *log = (GLchar*) malloc(logLength);
    glGetShaderInfoLog(vertShader, logLength, &logLength, log);
    printf("Vtx Shader compile log:%s\n", log);
    free(log);
  }

  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);
  if(status == 0)
  {
    printf("Failed to compile vtx shader:\n%s\n", sourceString);
    return 0;
  }

  free(sourceString);
  sourceString = NULL;

  // Attach the vertex shader to our program
  glAttachShader(prgName, vertShader);

  // Delete the vertex shader since it is now attached to the program, which will retain a reference to it
  glDeleteShader(vertShader);

  /////////////////////////////////////////
  // Specify and compile Fragment Shader //
  /////////////////////////////////////////

  int fragStringSize = sizeof(fragmentString) / sizeof(*fragmentString);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, fragStringSize, fragmentString, NULL);
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
  if(logLength > 0)
  {
    GLchar *log = (GLchar*)malloc(logLength);
    glGetShaderInfoLog(fragShader, logLength, &logLength, log);
    printf("Frag Shader compile log:\n%s\n", log);
    free(log);
  }

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
  if(status == 0)
  {
    printf("Failed to compile frag shader:\n%s\n", sourceString);
    return 0;
  }

  free(sourceString);
  sourceString = NULL;

  // Attach the fragment shader to our program
  glAttachShader(prgName, fragShader);

  // Delete the fragment shader since it is now attached
  // to the program, which will retain a reference to it
  glDeleteShader(fragShader);

  //////////////////////
  // Link the program //
  //////////////////////

  glLinkProgram(prgName);
  glGetProgramiv(prgName, GL_INFO_LOG_LENGTH, &logLength);
  if(logLength > 0)
  {
    GLchar *log = (GLchar*)malloc(logLength);
    glGetProgramInfoLog(prgName, logLength, &logLength, log);
    printf("Program link log:\n%s\n", log);
    free(log);
  }

  glGetProgramiv(prgName, GL_LINK_STATUS, &status);
  if(status == 0)
  {
    printf("Failed to link program");
    return 0;
  }

  glGetProgramiv(prgName, GL_INFO_LOG_LENGTH, &logLength);
  if(logLength > 0)
  {
    GLchar *log = (GLchar*)malloc(logLength);
    glGetProgramInfoLog(prgName, logLength, &logLength, log);
    printf("Program validate log:\n%s\n", log);
    free(log);
  }

  glUseProgram(prgName);
  // Setup common program input points
  GLint samplerLoc = glGetUniformLocation(prgName, "textureMap");
  assert(samplerLoc >= 0);// "Could not get sampler Uniform Index");
  // Indicate that the diffuse texture will be bound to texture unit 0
  GLint unit = 0;
  glUniform1i(samplerLoc, unit);

  GetCppGLError(__FILE__, __LINE__);
  return prgName;
}

} // end of namespace Launcher
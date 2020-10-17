#import "OpenGLRenderer.h"

@implementation OpenGLRenderer
{
  CGSize _viewSize;
  GLuint _programName;
  GLuint _vaoName;
  GLenum _baseMapTexTarget;
  GLuint _baseMapTexName;
}

// Indicies to which we will set vertex array attibutes
// See buildVAO and buildProgram
enum {
  POS_ATTRIB_IDX,
  TEXCOORD_ATTRIB_IDX
};

- (instancetype)init
{
  self = [super init];
  if(self)
  {
    NSLog(@"%s %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    // Build all of our objects and setup initial state here
    _vaoName = [self buildVAO];
    _programName = [self buildProgramShaders];
  }
  return self;
}

- (GLuint)buildVAO
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
  GetGLError();
  return vaoName;
}

- (void)destroyVAO:(GLuint)vaoName
{
  GLuint index;
  GLuint bufName;
  // Bind the VAO so we can get data from it
  glBindVertexArray(vaoName);
  // For every possible attribute set in the VAO, delete the attached buffer
  for(index = 0; index < 16; index++)
  {
    glGetVertexAttribiv(index , GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, (GLint*)&bufName);
    if(bufName)
    {
      glDeleteBuffers(1, &bufName);
    }
  }
  glDeleteVertexArrays(1, &vaoName);
  GetGLError();
}

- (GLuint)buildProgramShaders
{
  NSString *vertexString = @"#ifdef GL_ES\n"
                            "precision highp float;\n"
                            "#endif\n"
                            "#if __VERSION__ >= 140\n"
                            "in vec4  inPosition;\n"
                            "in vec2  inTexcoord;\n"
                            "out vec2 varTexcoord;\n"
                            "#else\n"
                            "attribute vec4 inPosition;\n"
                            "attribute vec2 inTexcoord;\n"
                            "varying vec2 varTexcoord;\n"
                            "#endif\n"
                            "void main (void)\n"
                            "{\n"
                            "    gl_Position = inPosition;\n"
                            "    varTexcoord = inTexcoord;\n"
                            "}";

  NSString *fragmentString = @"#ifdef GL_ES\n"
                              "precision highp float;\n"
                              "#endif\n"
                              "#if __VERSION__ >= 140\n"
                              "in vec2 varTexcoord;\n"
                              "out vec4 fragColor;\n"
                              "#else\n"
                              "varying vec2 varTexcoord;\n"
                              "#endif\n"
                              "uniform sampler2D textureMap;\n"
                              "void main (void)\n"
                              "{\n"
                              "    #if __VERSION__ >= 140\n"
                              "    fragColor = texture(textureMap, varTexcoord.st, 0.0);\n"
                              "    #else\n"
                              "    gl_FragColor = texture2D(textureMap, varTexcoord.st, 0.0);\n"
                              "    #endif\n"
                              "}";
  
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

  // Allocate memory for the source string including the version preprocessor information
  sourceString = malloc(vertexString.length + versionStringSize);

  // Prepend our vertex shader source string with the supported GLSL version so
  //  the shader will work on ES, Legacy, and OpenGL 3.2 Core Profile contexts
#if TARGET_IOS
  sprintf(sourceString, "#version %d es\n%s", version, vertexString.UTF8String);
#else
  sprintf(sourceString, "#version %d core\n%s", version, vertexString.UTF8String);
#endif
  
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const GLchar **)&(sourceString), NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

  if(logLength > 0)
  {
    GLchar *log = (GLchar*) malloc(logLength);
    glGetShaderInfoLog(vertexShader, logLength, &logLength, log);
    NSLog(@"Vtx Shader compile log:%s\n", log);
    free(log);
  }

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if(status == 0)
  {
    NSLog(@"Failed to compile vtx shader:\n%s\n", sourceString);
    return 0;
  }

  free(sourceString);
  sourceString = NULL;

  // Attach the vertex shader to our program
  glAttachShader(prgName, vertexShader);

  // Delete the vertex shader since it is now attached to the program, which will retain a reference to it
  glDeleteShader(vertexShader);

  /////////////////////////////////////////
  // Specify and compile Fragment Shader //
  /////////////////////////////////////////

  // Allocate memory for the source string including the version preprocessor information
  sourceString = malloc(fragmentString.length + versionStringSize);

  // Prepend our fragment shader source string with the supported GLSL version so
  //  the shader will work on ES, Legacy, and OpenGL 3.2 Core Profile contexts
#if TARGET_IOS
  sprintf(sourceString, "#version %d es\n%s", version, fragmentString.UTF8String);
#else
  sprintf(sourceString, "#version %d core\n%s", version, fragmentString.UTF8String);
#endif
  
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, (const GLchar **)&(sourceString), NULL);
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
  if(logLength > 0)
  {
    GLchar *log = (GLchar*)malloc(logLength);
    glGetShaderInfoLog(fragShader, logLength, &logLength, log);
    NSLog(@"Frag Shader compile log:\n%s\n", log);
    free(log);
  }

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
  if(status == 0)
  {
    NSLog(@"Failed to compile frag shader:\n%s\n", sourceString);
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
    NSLog(@"Program link log:\n%s\n", log);
    free(log);
  }

  glGetProgramiv(prgName, GL_LINK_STATUS, &status);
  if(status == 0)
  {
    NSLog(@"Failed to link program");
    return 0;
  }

  glGetProgramiv(prgName, GL_INFO_LOG_LENGTH, &logLength);
  if(logLength > 0)
  {
    GLchar *log = (GLchar*)malloc(logLength);
    glGetProgramInfoLog(prgName, logLength, &logLength, log);
    NSLog(@"Program validate log:\n%s\n", log);
    free(log);
  }

  glUseProgram(prgName);
  // Setup common program input points
  GLint samplerLoc = glGetUniformLocation(prgName, "textureMap");
  NSAssert(samplerLoc >= 0, @"Could not get sampler Uniform Index");
  // Indicate that the diffuse texture will be bound to texture unit 0
  GLint unit = 0;
  glUniform1i(samplerLoc, unit);

  GetGLError();
  return prgName;
}

- (void)draw:(GLuint)texName;
{
  glClearColor(0, 0, 0.5, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glUseProgram(_programName);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texName);
  glBindVertexArray(_vaoName);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  GetGLError();
}

- (void)resize:(CGSize)size
{
  _viewSize = size;
  glViewport(0, 0, size.width, size.height);
  float aspect = (float)size.width / size.height;
}

@end

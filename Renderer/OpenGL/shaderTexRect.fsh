/*
 <samplecode>
 <abstract>
 OpenGL fragment shader used to sample from a Rectangle texture
 </abstract>
 </samplecode>
 */
#ifdef GL_ES
precision highp float;
#endif


// Declare inputs and outputs
// varTexcoord : TexCoord for the fragment computed by the rasterizer based on
//               the varTexcoord values output in the vertex shader.
// gl_FragColor : Implicitly declared in fragments shaders less than 1.40.
//                Output color of our fragment.
// fragColor : Output color of our fragment.  Basically the same as gl_FragColor,
//             but we must explicitly declare this in shaders version 1.40 and
//             above.

#if __VERSION__ >= 140
in vec2      varTexcoord;
out vec4     fragColor;
#else
#extension GL_ARB_texture_rectangle : require
varying vec2 varTexcoord;
#endif

uniform vec2 textureDimensions;
uniform sampler2DRect baseMap;

void main (void)
{
    vec2 texCoord = vec2(varTexcoord.s, 1.0 - varTexcoord.t);
    vec2 recTexCoord = texCoord * textureDimensions;

    #if __VERSION__ >= 140
    fragColor = texture(baseMap, recTexCoord);
    #else
    gl_FragColor = texture2DRect(baseMap, recTexCoord);
    #endif
}

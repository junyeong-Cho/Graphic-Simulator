#version 300 es
precision highp float;

in vec2 vTextureCoordinates;

layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;
uniform vec3      uFogColor;
uniform float     uFogDensity;

void main()
{
    ///fFragmentColor    = texture(uTex2d, vTextureCoordinates);
    vec4 color     = texture(uTex2d, vTextureCoordinates);
    fFragmentColor = mix(color, uFogColor, uFogDensity); 
}

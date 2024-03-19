#version 300 es
precision highp float;

in vec2 vTextureCoordinates;

layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;
uniform vec3      uFogColor;
uniform float     uFogNear;
uniform float     uFogFar;

void main()
{
    fFragmentColor    = texture(uTex2d, vTextureCoordinates);
}

#version 300 es
precision highp float;

in vec2 fTextureCoordinates;

layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;

void main()
{
    fFragmentColor = texture(uTex2d, fTextureCoordinates);
}

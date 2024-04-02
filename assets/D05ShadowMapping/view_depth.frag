#version 300 es
precision highp float;

in vec2 vTextureCoordinates;

layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uShadowMap;

void main()
{
    fFragmentColor = texture(uShadowMap, vTextureCoordinates);
}

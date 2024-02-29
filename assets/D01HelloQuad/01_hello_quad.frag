#version 300 es
precision highp float;

in vec3 vColor;
in vec2 vTextureCoordinates;

uniform sampler2D uTex2d;

layout(location = 0) out vec4 fFragmentColor;

void main()
{
    vec3  color       = mix(vColor, texture(uTex2d, vTextureCoordinates).rgb, 0.5f);
    fFragmentColor    = vec4(color, 1.0);
}

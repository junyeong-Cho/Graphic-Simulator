#version 300 es
precision highp float;

in vec3 vColor;
in vec2 vTextureCoordinates;

uniform sampler2D uTex2d;
uniform float uTime;

layout(location = 0) out vec4 fFragmentColor;

void main()
{
    float delay = 0.7f;
    float colorChanger = (sin(uTime * delay) + 1.0f) / 2.0f; 
    vec3 color = mix(vColor, texture(uTex2d, vTextureCoordinates).rgb, colorChanger);
    fFragmentColor = vec4(color, 1.0f);
}

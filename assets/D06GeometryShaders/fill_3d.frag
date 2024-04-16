#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

uniform vec3 uDiffuseMaterial;

void main()
{
    fFragmentColor = vec4(uDiffuseMaterial, 1.0);
}

#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

uniform vec3 uDiffuseMaterial;

in vec4 gColor;

void main()
{
    fFragmentColor = gColor;

}

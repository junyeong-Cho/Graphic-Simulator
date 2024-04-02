#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

uniform vec3 uDiffuse;

void main()
{
    fFragmentColor = vec4(uDiffuse, 1.0);
}

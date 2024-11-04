#version 300 es
precision highp float;

in vec3 vColor;

layout(location = 0) out vec4 fFragmentColor;

void main()
{
    fFragmentColor = vec4(vColor, 1.0f);
}

#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vColor;

uniform mat4 uClipcoords;

void main()
{
    gl_Position = uClipcoords * vec4(aVertexPosition, 1.0);
    vColor = aVertexColor;
}

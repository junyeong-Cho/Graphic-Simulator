#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
// layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 vTextureCoordinates;

void main()
{
    gl_Position         = vec4(aVertexPosition, 1.0);
    vTextureCoordinates = aVertexTextureCoordinates;
}

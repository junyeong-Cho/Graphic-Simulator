#version 300 es

precision highp float;

layout(location = 0) in vec3 aVertexposition;
layout(location = 1) in vec3 aVertexNormal;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

void main()
{
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aVertexposition, 1.0);
}
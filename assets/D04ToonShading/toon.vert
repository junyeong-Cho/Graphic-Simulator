#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

void main()
{
    gl_Position                 = vec4(aVertexPosition, 1.0);
}

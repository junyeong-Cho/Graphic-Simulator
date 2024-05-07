#version 300 es

precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec2 aVertexTextureCoordinates;


uniform mat4 uProjection;

out vec2 fTextureCoordinates;


void main()
{
    // Further transform the vertex position to clip space.
    gl_Position = uProjection * vec4(aVertexPosition, 1.0);
    fTextureCoordinates = aVertexTextureCoordinates;
}
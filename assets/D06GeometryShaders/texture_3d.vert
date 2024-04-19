#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec3 vPosition;
out vec3 vNormals;
out vec2 vTextureCoordinates;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

void main() 
{
    vPosition = vec3(uModelMatrix * vec4(aVertexPosition, 1.0));
    vNormals = normalize(mat3(uModelMatrix) * aVertexNormal);
    vTextureCoordinates = aVertexTextureCoordinates;
    gl_Position = uProjection * uViewMatrix * vec4(vPosition, 1.0);
}

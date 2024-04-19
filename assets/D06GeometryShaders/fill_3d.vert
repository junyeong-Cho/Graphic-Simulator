#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTextureCoordinates;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

out vec3 vPosition;
out vec3 vNormal;
out vec4 vColor;

void main()
{
    vPosition = vec3(uModelMatrix * vec4(aVertexPosition, 1.0));
    gl_Position = vec4(vPosition, 1.0);

    vNormal = normalize(mat3(uModelMatrix) * aVertexNormal);

    vColor = vec4(abs(normalize(mat3(uModelMatrix) * aVertexNormal)), 1.0);

}

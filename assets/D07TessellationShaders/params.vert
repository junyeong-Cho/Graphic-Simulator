#version 450

precision highp float;

layout(location = 0) in vec3 aVertexPosition;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

out vec3 vPosition;

void main()
{
    vec4 worldPosition = uModelMatrix * vec4(aVertexPosition, 1.0);

    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);

    vPosition = vec3(uModelMatrix * vec4(aVertexPosition, 1.0));
}
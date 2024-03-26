#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

uniform vec3 uDiffuse;

uniform mat3 uNormalMatrix;

out vec3 EyespaceNormal;
out vec3 Diffuse;
out vec3 vPosition;


void main()
{
    EyespaceNormal = uNormalMatrix * aVertexNormal;

    gl_Position    = uProjection * uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);

    Diffuse        = uDiffuse;

    vPosition      = ((uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0)).xyz);

}

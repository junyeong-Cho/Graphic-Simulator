#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;

out vec3 vNormalInViewSpace;
out vec3 vPositionInViewSpace;
out vec4 vPositionInShadowSpace;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform mat4 uShadowMatrix;

void main()
{
    // TODO set gl_Position to position in clip space
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);

    // TODO set vNormalInViewSpace to be normal in view space 
    vNormalInViewSpace = uNormalMatrix * aVertexNormal;

    // TODO set vPositionInViewSpace to be position in view space
    vPositionInViewSpace = (uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0)).xyz;

    // TODO set vPositionInShadowSpace to be in shadow space
    vPositionInShadowSpace = uShadowMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);
}

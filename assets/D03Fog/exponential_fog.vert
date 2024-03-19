#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 vTextureCoordinates;
out vec3 vPosition;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

void main()
{
    //gl_Position                 = vec4(aVertexPosition, 1.0);
    //vTextureCoordinates         = aVertexTextureCoordinates;

    gl_Position                 = uProjection * uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);
    vTextureCoordinates         = aVertexTextureCoordinates;

    //vPosition = (u_worldview * position).xyz;
    vPosition = ((uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0)).xyz);


}

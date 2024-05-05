#version 450

precision highp float;

layout(location = 0) in vec3 aVertexPosition;

uniform mat4 uProjection;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

out vec3 vPosition; 

void main()
{
    
    // Transform the vertex position from model to world space.
    vPosition = vec3(uModelMatrix * vec4(aVertexPosition, 1.0));

    // Further transform the vertex position to clip space.
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(vPosition, 1.0);
}

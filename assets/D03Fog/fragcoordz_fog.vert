#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 vTextureCoordinates;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

void main()
{
	// Multiply the position by the matrix.
	gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aVertexPosition, 1.0);

	// Pass the texcoord to the fragment shader.
    vTextureCoordinates         = aVertexTextureCoordinates;
}

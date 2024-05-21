#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform float uTileScale;

out vec2 TexCoord;

void main() 
{
    TexCoord = aTexCoord * uTileScale;
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(aPos, 1.0);
}

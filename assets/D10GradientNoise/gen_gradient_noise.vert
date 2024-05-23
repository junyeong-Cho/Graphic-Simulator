#version 450

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 TexCoord;

void main()
{
    TexCoord = aVertexTextureCoordinates;
    gl_Position = vec4(aVertexPosition, 1.0);
}

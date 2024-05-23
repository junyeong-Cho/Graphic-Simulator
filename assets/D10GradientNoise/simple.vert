#version 450

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 TexCoord;

uniform mat4 uProjection;
uniform float uTileScale;

void main()
{
    gl_Position = uProjection * vec4(aVertexPosition, 1.0);

    TexCoord = aVertexTextureCoordinates * uTileScale;
    
}

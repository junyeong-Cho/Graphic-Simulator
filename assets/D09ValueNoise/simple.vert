#version 330 core


layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;


uniform mat4 uProjection;
uniform vec2 uTileScale;


out vec2 vTexCoord;

void main()
{

    gl_Position = uProjection * vec4(aVertexPosition, 1.0);


    vTexCoord = aVertexTextureCoordinates * uTileScale;
}

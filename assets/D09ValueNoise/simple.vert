#version 330 core


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;


uniform mat4 uProjection;
uniform vec2 uTileScale;


out vec2 vTexCoord;

void main()
{

    gl_Position = uProjection * vec4(aPosition, 1.0);


    vTexCoord = aTexCoord * uTileScale;
}

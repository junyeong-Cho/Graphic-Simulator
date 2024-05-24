#version 450

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 TexCoord;

uniform mat4 uProjection;
uniform float uTileScale;

uniform mat4 uNormalMatrix;  
uniform vec3 uAmbient;       
uniform vec3 uDiffuse;       
uniform vec3 uSpecularColor; 
uniform float uShininess;    

void main()
{
    gl_Position = uProjection * vec4(aVertexPosition, 1.0);

    TexCoord = aVertexTextureCoordinates * uTileScale;
    
}

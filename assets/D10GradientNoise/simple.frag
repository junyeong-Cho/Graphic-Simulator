#version 450

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform mat4 uNormalMatrix; 
uniform vec3 uAmbient;      
uniform vec3 uDiffuse;      
uniform vec3 uSpecularColor;
uniform float uShininess;   

void main()
{
    FragColor = vec4(texture(uTexture, TexCoord).rrr, 1.0f);
}

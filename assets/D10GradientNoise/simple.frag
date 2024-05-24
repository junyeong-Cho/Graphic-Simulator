#version 450

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
    FragColor = vec4(texture(uTexture, TexCoord).rrr, 1.0f);
}

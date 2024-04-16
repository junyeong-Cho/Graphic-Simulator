#version 300 es
precision highp float;

in vec2 fTextureCoordinates;
in float fHeight;

layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;

void main() 
{
    vec4 textureColor = texture(uTex2d, fTextureCoordinates);
    vec3 baseColor = mix(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), fHeight);
    fFragmentColor = vec4(textureColor.rgb * baseColor, 1.0);
}

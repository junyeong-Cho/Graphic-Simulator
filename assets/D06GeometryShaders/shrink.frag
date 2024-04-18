#version 300 es
precision highp float;

in vec2 fTextureCoordinates;


layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;
uniform vec3 uDiffuseMaterial;

void main() 
{
    vec4 texColor = texture(uTex2d, fTextureCoordinates);
    fFragmentColor = vec4(texColor.rgb * uDiffuseMaterial, texColor.a);
}

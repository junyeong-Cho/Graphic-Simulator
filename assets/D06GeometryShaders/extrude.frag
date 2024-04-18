#version 300 es
precision highp float;

in vec2 fTextureCoordinates;
in float fLightIntensity;

layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;
uniform vec3 uDiffuseMaterial;

void main() 
{
    vec4 texColor = texture(uTex2d, fTextureCoordinates);
    vec3 color = mix(vec3(0.0), texColor.rgb, fLightIntensity);
    fFragmentColor = vec4(color * uDiffuseMaterial, texColor.a);
}

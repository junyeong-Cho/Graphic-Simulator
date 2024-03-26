#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

uniform vec3      uFogColor;
uniform float     uFogNear;
uniform float     uFogFar;
uniform vec3      uDiffuse;
uniform vec3      uAmbient;
uniform float     uShininess;
uniform vec3      uSpecularColor;
uniform vec3      uLightDirection;
uniform sampler2D uGradientTexture;
uniform bool      uEnableAntiAliasing;
uniform int       uImageWidth;

void main()
{
    fFragmentColor = fFragmentColor = texture(uGradientTexture, vec2(0,0));
}

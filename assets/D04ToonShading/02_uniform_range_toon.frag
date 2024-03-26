#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

uniform vec3  uFogColor;
uniform float uFogNear;
uniform float uFogFar;
uniform vec3  uDiffuse;
uniform vec3  uAmbient;
uniform float uShininess;
uniform vec3  uSpecularColor;
uniform vec3  uLightDirection;

uniform int  uNumDiffuseChunks;
uniform bool uEnableAntiAliasing;

void main()
{
   fFragmentColor = vec4(uDiffuse, 1.0);
}

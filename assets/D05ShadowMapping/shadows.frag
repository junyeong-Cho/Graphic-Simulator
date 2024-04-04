#version 300 es
precision highp float;
precision highp sampler2DShadow;

layout(location = 0) out vec4 fFragmentColor;

in vec3 vNormalInViewSpace;
in vec3 vPositionInViewSpace;
in vec4 vPositionInShadowSpace;

uniform sampler2DShadow uShadowMap;
uniform vec3  uFogColor;
uniform float uFogDensity;
uniform vec3  uDiffuse;
uniform vec3  uAmbient;
uniform float uShininess;
uniform vec3  uSpecularColor;
uniform vec3  uLightPosition;
uniform bool  uDoShadowBehindLight;

void main()
{
    vec3 n = normalize(vNormalInViewSpace);
    vec3 l = normalize(uLightPosition - vPositionInViewSpace);
    float nl = max(dot(n, l), 0.0);
    vec3 eye = vec3(0, 0, 1);
    vec3 h = normalize(l + eye);
    float spec = max(0.0, dot(n, h));
    spec = pow(spec, uShininess);
    vec3 diffuse = nl * uDiffuse;

    float shadow = textureProj(uShadowMap, vPositionInShadowSpace);

    if (!uDoShadowBehindLight && vPositionInShadowSpace.z < 0.0) {
        shadow = 1.0; // Full light behind the light source
    }

    vec3 color = uAmbient + shadow * (diffuse + spec * uSpecularColor);

    float distance = length(vPositionInViewSpace);
    float fogAmount = 1.0 - exp(-pow((distance * uFogDensity), 2.0));
    fFragmentColor = vec4(mix(color, uFogColor, fogAmount), 1.0);
}
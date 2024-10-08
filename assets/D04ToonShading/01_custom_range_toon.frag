#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

in vec3 EyespaceNormal;
in vec3 Diffuse;
in vec3 vPosition;

uniform vec3  uFogColor;
uniform float uFogNear;
uniform float uFogFar;

uniform vec3  uDiffuse;
uniform vec3  uAmbient;
uniform float uShininess;
uniform vec3  uSpecularColor;
uniform vec3  uLightDirection;

uniform float uLevel1;
uniform float uLevel2;
uniform float uLevel3;
uniform float uMax;
uniform bool  uEnableAntiAliasing;

float stepmix(float edge0, float edge1, float E, float x) 
{
    float T = clamp(0.5 * (x - edge0) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

void main() {
    vec3 N   = normalize(EyespaceNormal);
    vec3 L   = normalize(uLightDirection);
    vec3 Eye = vec3(0, 0, 1);
    vec3 H   = normalize(L + Eye);

    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, uShininess);

    
    float Edf = fwidth(df);


    if(uEnableAntiAliasing)
    {
        if (df > uLevel1 - Edf && df < uLevel1 + Edf) df      = stepmix(uLevel1, uLevel2, Edf, df);
        else if (df > uLevel2 - Edf && df < uLevel2 + Edf) df = stepmix(uLevel2, uLevel3, Edf, df);
        else if (df > uLevel3 - Edf && df < uLevel3 + Edf) df = stepmix(uLevel3, uMax, Edf, df);
        else if (df < uLevel1) df = uLevel1;
        else if (df < uLevel2) df = uLevel2;
        else if (df < uLevel3) df = uLevel3;
        else df = uMax;

    }
    else
    {
        if (df < uLevel1) df = uLevel1;
        else if (df < uLevel2) df = uLevel2;
        else if (df < uLevel3) df = uLevel3;
        else df = uMax;
        
        sf = step(0.5, sf);

    }

    Edf = fwidth(sf);
    if(uEnableAntiAliasing)
    {
        if (sf > 0.5 - Edf && sf < 0.5 + Edf) 
        {
            sf = smoothstep(0.5 - Edf, 0.5 + Edf, sf);
        } 
        else 
        {
            sf = step(0.5, sf);
        }
    }
    else
    {
        sf = step(0.5, sf);
    }


    vec3 color = uAmbient + df * Diffuse + sf * uSpecularColor;

    float fogDistance = length(vPosition);
    float fogAmount   = smoothstep(uFogNear, uFogFar, fogDistance);

    fFragmentColor = mix(vec4(color, 1.0), vec4(uFogColor, 1.0f), fogAmount);
}

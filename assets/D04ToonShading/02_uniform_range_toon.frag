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

uniform int   uNumDiffuseChunks;
uniform bool  uEnableAntiAliasing;

void main()
{
    vec3 N = normalize(EyespaceNormal);
    vec3 L = normalize(uLightDirection);
    vec3 E = vec3(0, 0, 1);
    vec3 H = normalize(L + E);

    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, uShininess);


    float chunkSize = -(1.0 / float(uNumDiffuseChunks));
    df = floor(df / chunkSize) * chunkSize;

    
    // Specular anti-aliasing
    if (uEnableAntiAliasing) 
    {
        float Edf = fwidth(sf);
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

#version 300 es
precision highp float;

layout(location = 0) out vec4 fFragmentColor;

in vec3 EyespaceNormal;
in vec3 Diffuse;
in vec3 vPosition;

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

float smoothFloor(float x)
{
    return ( floor(x-fwidth(x)/2.) + max( 0., 1.-fract(-x+fwidth(x)/2.) / fwidth(x) )) ;
}

void main()
{
    vec3 N = normalize(EyespaceNormal);
    vec3 L = normalize(uLightDirection);
    vec3 E = vec3(0, 0, 1);
    vec3 H = normalize(L + E);

    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, uShininess);

    
    float textureBrightnessSum = 0.0;
    for(int i = 0; i < uImageWidth; ++i) 
    {
        vec4 texColor    = texelFetch(uGradientTexture, ivec2(i, 0), 0);
        float brightness = dot(texColor.rgb, vec3(1.0, 1.0, 1.0));
        textureBrightnessSum += brightness;
    }
    float averageBrightness = textureBrightnessSum / float(uImageWidth);

    
    if(uEnableAntiAliasing)
    {
        float numSteps = smoothFloor(10.0 * averageBrightness);
                    df = smoothFloor(df * numSteps) / numSteps;
    }
    else
    {
        float numSteps = floor(10.0 * averageBrightness);
                    df = floor(df * numSteps) / numSteps;
    }
    
    
    int texelIndex = int(df * float(uImageWidth));
    vec4 texelColor = texelFetch(uGradientTexture, ivec2(texelIndex, 0), 0);

    vec4 shapeColor = vec4(uDiffuse * texelColor.rgb, 1.0);

    float Edf = fwidth(sf);
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



    vec4 color = vec4(uAmbient, 1.0) + shapeColor + vec4(sf * uSpecularColor, 1.0);



    // Fog effect
    float fogDistance = length(vPosition);
    float fogAmount = smoothstep(uFogNear, uFogFar, fogDistance);
    vec4 finalColor = mix(color, vec4(uFogColor, 1.0), fogAmount);

    fFragmentColor = finalColor;
}

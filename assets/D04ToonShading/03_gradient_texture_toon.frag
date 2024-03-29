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
    float sampleStep = 1.0 / float(uImageWidth); 

    for(int i = 0; i < uImageWidth; i++) 
    {
        float texCoordX = float(i) * sampleStep;
        vec4 texColor = texture(uGradientTexture, vec2(texCoordX, 0.5));
        float brightness = dot(texColor.rgb, vec3(0.299, 0.587, 0.114));
        textureBrightnessSum += brightness;
    }

        float averageBrightness = textureBrightnessSum / float(uImageWidth);


    float numSteps = floor(10.0 * averageBrightness);
    df = floor(df * numSteps) / numSteps;



    vec4 shapeColor = vec4(uDiffuse * texture(uGradientTexture, vec2(df, 0.5)).rgb , 1.0);

    vec4 color = vec4(uAmbient, 1.0) + shapeColor + vec4(sf * uSpecularColor, 1.0);



    float fogDistance = length(vPosition);
    float fogAmount   = smoothstep(uFogNear, uFogFar, fogDistance);

    vec4 finalColor = mix(color, vec4(uFogColor, 1.0), fogAmount);

    fFragmentColor = finalColor;


    //fFragmentColor = vec4(uDiffuse * texture(uGradientTexture, vec2(df, 0.5)).rgb , 1.0);
    





    
    // if (uEnableAntiAliasing) 
    // {
    //     float Edf = fwidth(df);
    //     df = df + 0.5 * Edf;
    // }

    
    // float Esf = fwidth(sf);
    // if (uEnableAntiAliasing) 
    // {
    //     if (sf > 0.5 - Esf && sf < 0.5 + Esf) 
    //     {
    //         sf = smoothstep(0.5 - Esf, 0.5 + Esf, sf);
    //     } 
    //     else 
    //     {
    //         sf = step(0.5, sf);
    //     }
    // }

    
    // vec3 color = uAmbient + df * Diffuse * textureColor.rgb + sf * uSpecularColor;

    
    // float fogDistance = length(vPosition);
    // float fogAmount = smoothstep(uFogNear, uFogFar, fogDistance);

    
    // vec4 finalColor = mix(vec4(color, 1.0) * textureColor, vec4(uFogColor, 1.0), fogAmount);

    // fFragmentColor = finalColor;
}

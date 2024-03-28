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
    vec3 normalizedLightDirection = normalize(uLightDirection);
    vec3 normal = normalize(EyespaceNormal);
    
    float diff = max(dot(normal, normalizedLightDirection), 0.0);
    
    vec3 ambient = uAmbient * uDiffuse;
    vec3 diffuse = diff * uDiffuse;
    
    vec3 viewDir = normalize(-vPosition); // Assuming the eye is at the origin

    vec3 reflectDir = reflect(-normalizedLightDirection, normal);
    
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float specularComponent = pow(specAngle, uShininess);
    

    vec3 specular = specularComponent * uSpecularColor;
    
    vec3 lighting = ambient + diffuse + specular;
    
    float lightingFactor = length(lighting); 
    vec2 textureCoord = vec2(lightingFactor, 0.5); 
    
    vec4 textureColor = texture(uGradientTexture, textureCoord);
    
    float fogDistance = length(vPosition);
    float fogAmount = smoothstep(uFogNear, uFogFar, fogDistance);
    
    vec4 finalColor = mix(textureColor, vec4(uFogColor, 1.0), fogAmount);
    
    fFragmentColor = finalColor;
}

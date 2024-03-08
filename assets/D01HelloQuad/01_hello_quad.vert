#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec3 vColor;
out vec2 vTextureCoordinates;

uniform float uTime;

void main()
{
    float timeMod = mod(uTime, 6.0f);                   
    float bounceEffect = abs(sin(timeMod * 3.14159f));

    vec3 modifiedPosition = aVertexPosition;
    modifiedPosition.x += bounceEffect * (aVertexPosition.x > 0.0f ? 1.0f : -1.0f);
    modifiedPosition.y += bounceEffect * (aVertexPosition.y > 0.0f ? 1.0f : -1.0f); 

    float interpolant = smoothstep(-1.0f, 1.0f, sin(uTime * 1.1f)); 
    float scale = mix(0.3f, 1.1f, interpolant); 
    gl_Position = vec4(modifiedPosition * scale, 1.0f); 

    vColor = aVertexColor;
    vTextureCoordinates = aVertexTextureCoordinates;
}
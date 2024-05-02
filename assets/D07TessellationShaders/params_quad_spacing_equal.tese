#version 450

layout(quads) in;
layout(equal_spacing) in;

in vec3 tcPosition[];

out vec3 tePosition;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    
    // bilinearly interpolated object-space position
    vec3 lowerPos = mix(tcPosition[0], tcPosition[1], u);
    vec3 upperPos = mix(tcPosition[2], tcPosition[3], u);
    vec3 position = mix(lowerPos, upperPos, v);

    tePosition = position;
    
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(position, 1.0);

}

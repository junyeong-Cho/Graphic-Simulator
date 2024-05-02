#version 450

layout(quads) in;
layout(fractional_odd_spacing) in;

in vec3 tcPosition[];

out vec3 tePosition;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    
    vec3 p0 = mix(tcPosition[0], tcPosition[1], u);
    vec3 p1 = mix(tcPosition[3], tcPosition[2], u);
    tePosition = mix(p0, p1, v);
    
    vec4 worldPosition = uModelMatrix * vec4(tePosition, 1.0);
    gl_Position = uProjection * uViewMatrix * worldPosition;
}

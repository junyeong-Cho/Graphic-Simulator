#version 450

layout(isolines) in;
layout(fractional_odd_spacing) in;

in vec3 tcPosition[];
out vec3 tePosition;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
    float u = gl_TessCoord.x;
    
    tePosition = mix(tcPosition[0], tcPosition[1], u);
    vec4 worldPosition = uModelMatrix * vec4(tePosition, 1.0);
    gl_Position = uProjection * uViewMatrix * worldPosition;
}

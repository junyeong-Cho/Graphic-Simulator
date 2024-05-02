#version 450

layout(triangles) in;
layout(fractional_even_spacing) in;

in vec3 tcPosition[];
out vec3 tePosition;

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main() 
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = 1.0 - u - v;

    tePosition  = u * tcPosition[0] + v * tcPosition[1] + w * tcPosition[2];
    vec4 worldPosition = uModelMatrix * vec4(tePosition, 1.0);
    gl_Position = uProjection * uViewMatrix * worldPosition;
}

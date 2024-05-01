#version 450

layout(triangles) in;
layout(equal_spacing) in;

in vec3 tcPosition[];
out vec3 tePosition;

void main() 
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = 1.0 - u - v;
    tePosition = u * tcPosition[0] + v * tcPosition[1] + w * tcPosition[2];
    gl_Position = u * gl_in[0].gl_Position + v * gl_in[1].gl_Position + w * gl_in[2].gl_Position;
}

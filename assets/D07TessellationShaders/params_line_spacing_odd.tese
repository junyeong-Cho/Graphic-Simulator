#version 450

layout(isolines) in;
layout(fractional_odd_spacing) in;

in vec3 tcPosition[];

out vec3 tePosition;

void main()
{
    float u = gl_TessCoord.x;
    tePosition = mix(tcPosition[0], tcPosition[1], u);
    gl_Position = gl_in[0].gl_Position * (1.0 - u) + gl_in[1].gl_Position * u;
}

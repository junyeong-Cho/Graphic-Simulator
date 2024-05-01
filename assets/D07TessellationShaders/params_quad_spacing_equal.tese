#version 450

layout(quads) in;
layout(equal_spacing) in;

in vec3 tcPosition[];

out vec3 tePosition;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    
    vec3 p0 = mix(tcPosition[0], tcPosition[1], u);
    vec3 p1 = mix(tcPosition[3], tcPosition[2], u);
    tePosition = mix(p0, p1, v);
    
    gl_Position = mix(mix(gl_in[0].gl_Position, gl_in[1].gl_Position, u),
                      mix(gl_in[3].gl_Position, gl_in[2].gl_Position, u), v);
}

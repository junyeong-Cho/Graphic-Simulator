#version 300 es

layout(lines) in;
layout(points, max_vertices = 128) out;

void main()
{
    vec4 start = gl_in[0].gl_Position;
    vec4 end   = gl_in[1].gl_Position;
    vec4 diff  = (end - start) / 127.0;

    gl_PointSize = 6.0;
    gl_Position  = start;
    EmitVertex();

    gl_PointSize = 1.0;
    for (int i = 1; i < 127; i++)
    {
        gl_Position = start + float(u) * diff;
        EmitVertex();
    }

    gl_PointSize = 6.0;
    gl_Position  = end;
    EmitVertex();
}
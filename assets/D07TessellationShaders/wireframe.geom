#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 tePosition[];

void main()
{
    for(int i = 0; i < 3; i++)
    {
        gl_Position = vec4(tePosition[i], 1.0);
        EmitVertex();
        gl_Position = vec4(tePosition[(i + 1) % 3], 1.0);
        EmitVertex();
        
        EndPrimitive();
    }
}

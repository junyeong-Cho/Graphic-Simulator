#version 300 es
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

void main() 
{
    for (int i = 0; i < 3; i++) 
    {
        // Emit the current vertex
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();

        // Emit the next vertex in the triangle, wrapping around
        gl_Position = gl_in[(i+1) % 3].gl_Position;
        EmitVertex();

        // Finish current line strip segment
        EndPrimitive();
    }
}

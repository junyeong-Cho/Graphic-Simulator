#version 300 es

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 vTextureCoordinates[];
out vec2 fTextureCoordinates;

void main()
{
    for(int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        EmitVertex();
    }
    EndPrimitive();
}

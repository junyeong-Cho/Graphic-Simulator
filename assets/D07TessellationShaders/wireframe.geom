#version 450 core
precision highp float;


layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 tePosition[3];
in vec3 tePatchDistance[3];
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

void main()
{
    for(int i = 0; i < 3; ++i)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();

        int nextIndex = (i + 1) % 3;
        gl_Position = gl_in[nextIndex].gl_Position;
        EmitVertex();
    }

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    EndPrimitive();
}
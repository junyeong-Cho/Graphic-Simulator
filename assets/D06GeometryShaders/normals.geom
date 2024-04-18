#version 300 es
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

// Define the length of the normal lines for visualization
uniform float uNormalLength = 0.005;

in vec3 vNormals[3];
in vec4 vColor[3];

void main() 
{
    for (int i = 0; i < 3; i++) 
    {
        // Start from the position of the current vertex
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();

        // End at the position plus the normal scaled by uNormalLength
        vec4 normalOffset = vec4(vNormals[i] * 0.04, 0.0);
        gl_Position = gl_in[i].gl_Position + normalOffset;
        EmitVertex();

        // Finish this line segment
        EndPrimitive();
    }
}
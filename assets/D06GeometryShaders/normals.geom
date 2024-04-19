#version 450 core
precision highp float;

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vPosition[];
in vec3 vNormal[];

uniform mat4 uProjection;
uniform mat4 uViewMatrix;

void main()
{
    const float normalLength = 0.05; 

    for(int i = 0; i < 3; ++i)
    {
        
        gl_Position =  gl_in[i].gl_Position;
        EmitVertex();
        
        // Calculate the end point of the normal line
        vec3 endNormal = vNormal[i] * normalLength;
        gl_Position = (gl_in[i].gl_Position + vec4(endNormal, 0.0));
        EmitVertex();

        
        EndPrimitive();
    }
}




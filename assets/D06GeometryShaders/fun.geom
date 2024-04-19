#version 300 es

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// Inputs from vertex shader
in vec3 vPosition[];
in vec3 vNormals[];
in vec2 vTextureCoordinates[];

// Outputs to fragment shader
out vec2 fTextureCoordinates;

uniform float uTime; 

void main()
{
    for(int i = 0; i < 3; ++i)
    {
        // Calculate twist factor based on the y-coordinate of the vertex position and time
        float twist = sin(vPosition[i].y * 2.0 + uTime) * 0.5;

        
        float s = sin(twist);
        float c = cos(twist);
        vec4 twistedPosition = vec4
        (
            c * gl_in[i].gl_Position.x - s * gl_in[i].gl_Position.y,
            s * gl_in[i].gl_Position.x + c * gl_in[i].gl_Position.y,
            gl_in[i].gl_Position.z,
            gl_in[i].gl_Position.w
        );


        gl_Position = twistedPosition;

        fTextureCoordinates = vTextureCoordinates[i];

        EmitVertex();
    }
    EndPrimitive();
}

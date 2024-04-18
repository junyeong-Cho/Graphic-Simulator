#version 300 es
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 vNormals[];
out vec3 fColor;

uniform mat4 uModelMatrix;
uniform float uNormalLength = 0.1;  

void main() 
{
    vec3 normalColor = vec3(0.0, 0.0, 0.0);  

    for (int i = 0; i < 3; i++) 
    {
        vec4 worldPosition = uModelMatrix * gl_in[i].gl_Position;
        vec3 normal = normalize(mat3(uModelMatrix) * vNormals[i]) * uNormalLength;

        
        gl_Position = worldPosition;
        fColor = normalColor;
        EmitVertex();

        gl_Position = worldPosition + vec4(normal, 0.0);
        fColor = normalColor;
        EmitVertex();

        EndPrimitive();
    }
}

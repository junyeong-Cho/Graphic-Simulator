#version 300 es
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 vTextureCoordinates[];
out vec2 fTextureCoordinates;

uniform mat4 uModelMatrix;
uniform float uTime; // Time variable to animate the explosion

void main() 
{
    vec3 centroid = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;
    float timeFactor = sin(uTime) * 0.5 + 0.5; // Normalized time factor for a pulsating effect

    for (int i = 0; i < 3; ++i) 
    {
        vec3 direction = normalize(gl_in[i].gl_Position.xyz - centroid);
        float magnitude = distance(gl_in[i].gl_Position.xyz, centroid) * timeFactor * 0.1;
        vec3 newPosition = gl_in[i].gl_Position.xyz + direction * magnitude;

        gl_Position = uModelMatrix * vec4(newPosition, 1.0);
        fTextureCoordinates = vTextureCoordinates[i];
        EmitVertex();
    }
    EndPrimitive();
}

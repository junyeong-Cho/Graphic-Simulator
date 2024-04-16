#version 300 es
layout(points) in;
layout(line_strip, max_vertices = 50) out; // 16 lines per point, with 2 vertices each

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform float uTime; // Animated time component

float PI = 3.1415926535897932384626433832795;

void main() {
    float radius = 0.1 * (sin(uTime) + 1.5); // Animated radius
    vec4 centralPosition = uProjection * uViewMatrix * uModelMatrix * gl_in[0].gl_Position;

    for (int i = 0; i < 16; ++i) 
    {
        float angle = 2.0 * PI * float(i) / 16.0;
        vec4 offset = vec4(radius * cos(angle), radius * sin(angle), 0.0, 0.0);
        gl_Position = centralPosition;
        EmitVertex();
        gl_Position = centralPosition + offset;
        EmitVertex();
        EndPrimitive();
    }
}

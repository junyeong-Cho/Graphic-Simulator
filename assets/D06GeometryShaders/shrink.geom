#version 300 es
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform float uShrinkFactor;  // Shrink factor from 0 (fully shrunk to center) to 1 (original shape)

void main() {
    vec3 centroid = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0;

    for (int i = 0; i < 3; i++) {
        vec3 newPosition = mix(centroid, gl_in[i].gl_Position.xyz, uShrinkFactor);
        gl_Position = vec4(newPosition, 1.0) * gl_in[i].gl_Position.w;
        EmitVertex();
    }
    EndPrimitive();
}

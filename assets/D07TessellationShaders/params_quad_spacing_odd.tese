#version 450

layout (quads, fractional_odd_spacing) in;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 p0 = mix(gl_in[0].gl_Position, gl_in[1].gl_Position, u);
    vec4 p1 = mix(gl_in[2].gl_Position, gl_in[3].gl_Position, u);

    gl_Position = mix(p0, p1, v);
}

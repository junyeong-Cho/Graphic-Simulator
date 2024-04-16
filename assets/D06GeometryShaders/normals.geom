#version 300 es
layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 vNormal[];

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform float uNormalLength;  // A uniform to control the length of the normal lines

void main() {
    vec3 normalEnd;
    vec4 worldSpacePos;
    vec4 clipSpaceNormalEnd;

    for (int i = 0; i < 3; i++) 
    {
        // Convert vertex position to world space
        worldSpacePos = uModelMatrix * vec4(gl_in[i].gl_Position.xyz, 1.0);
        normalEnd = worldSpacePos.xyz + vNormal[i] * uNormalLength;

        // Convert the end of the normal to clip space
        clipSpaceNormalEnd = uProjection * uViewMatrix * vec4(normalEnd, 1.0);

        // Emit the position of the vertex
        gl_Position = uProjection * uViewMatrix * worldSpacePos;
        EmitVertex();

        // Emit the position of the normal's end
        gl_Position = clipSpaceNormalEnd;
        EmitVertex();

        // Finish current line strip segment
        EndPrimitive();
    }
}

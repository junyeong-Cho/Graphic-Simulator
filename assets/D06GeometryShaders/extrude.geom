#version 300 es
layout(triangles) in;
layout(triangle_strip, max_vertices = 14) out;

in vec2 vTextureCoordinates[];
out vec2 fTextureCoordinates;
out float fHeight;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform float uExtrudeLength;
uniform bool uUseFlatNormals;

void main() {
    vec3 normal;
    if (uUseFlatNormals) 
    {
        vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        normal = normalize(cross(edge1, edge2));
    } 
    else 
    {
        normal = normalize(gl_in[0].gl_Normal + gl_in[1].gl_Normal + gl_in[2].gl_Normal);
    }

    vec4 baseVertices[3];
    vec4 extrudedVertices[3];

    for (int i = 0; i < 3; i++) 
    {
        baseVertices[i] = uProjection * uViewMatrix * uModelMatrix * gl_in[i].gl_Position;
        extrudedVertices[i] = baseVertices[i] + vec4(normal * uExtrudeLength, 0.0);

        // Emit the base vertex
        fTextureCoordinates = vTextureCoordinates[i];
        fHeight = 0.0;
        gl_Position = baseVertices[i];
        EmitVertex();

        // Emit the extruded vertex
        fTextureCoordinates = vTextureCoordinates[i];
        fHeight = 1.0;
        gl_Position = extrudedVertices[i];
        EmitVertex();

        if (i < 2) 
        {
            // Repeat the first two vertices to continue the strip
            EndPrimitive();
        }
    }

    // Close the top of the extrusion
    for (int i = 0; i < 3; i++) 
    {
        fTextureCoordinates = vTextureCoordinates[i];
        fHeight = 1.0;
        gl_Position = extrudedVertices[i];
        EmitVertex();
    }

    // Emit the last vertex to complete the top triangle
    EndPrimitive();
}

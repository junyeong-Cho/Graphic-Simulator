#version 450

layout (triangles , fractional_even_spacing) in;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

in vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];

    tePatchDistance = gl_TessCoord;
    tePosition = p0 + p1 + p2;
    gl_Position = uModelMatrix * uProjection * uViewMatrix * vec4(tePosition, 1.0);
}
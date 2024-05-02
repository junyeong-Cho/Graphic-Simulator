#version 450

layout(lines) in;
layout(points, max_vertices = 128) out;

in vec3 tePosition[];

uniform mat4 uProjection;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

void main()
{
    vec3 start = tePosition[0];
    vec3 end = tePosition[1];
    vec3 diff = (end - start) / 126.0;

    gl_PointSize = 6.0;
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(start, 1.0);
    EmitVertex();
       
    gl_PointSize = 1.0;
    for (int i = 1; i < 127; i++)
    {
        vec3 pos = start + float(i) * diff;
        gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(pos, 1.0);
        EmitVertex();
    }

    gl_PointSize = 6.0;
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(end, 1.0);
    EmitVertex();

    EndPrimitive();
}

#version 450

layout(vertices = 3) out;

in vec3 vPosition[];
out vec3 tcPosition[];

uniform vec2 uInsideTessellation;
uniform vec4 uOutsideTessellation;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID];

    if (ID == 0) 
    {
        gl_TessLevelInner[0] = uInsideTessellation.x;
        gl_TessLevelOuter[0] = uOutsideTessellation.x;
        gl_TessLevelOuter[1] = uOutsideTessellation.y;
        gl_TessLevelOuter[2] = uOutsideTessellation.z;
    }
}

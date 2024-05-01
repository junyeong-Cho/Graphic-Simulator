#version 450

layout(vertices = 2) out;

in vec3 vPosition[];
out vec3 tcPosition[];

uniform float TessLevelOuter;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID];

    if (ID == 0)
    {
        gl_TessLevelOuter[0] = TessLevelOuter;
        gl_TessLevelOuter[1] = TessLevelOuter;
    }
}

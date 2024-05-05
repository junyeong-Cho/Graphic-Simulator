#version 450


layout (vertices = 3) out;

in vec3 vPosition[];
out vec3 tcPosition[];

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

uniform vec4 uOutsideTessellation;
uniform vec2 uInsideTessellation;

void main() {

    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID]; 

    if(gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = uInsideTessellation[0];
        gl_TessLevelOuter[0] = uOutsideTessellation[0];
        gl_TessLevelOuter[1] = uOutsideTessellation[1];
        gl_TessLevelOuter[2] = uOutsideTessellation[2];
    }
}

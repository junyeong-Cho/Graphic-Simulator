#version 450

layout (vertices = 4) out;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

uniform vec4 uOutsideTessellation;

void main() {
    
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if(gl_InvocationID ==0)
    {
        gl_TessLevelOuter[0] = uOutsideTessellation[0]; 
        gl_TessLevelOuter[1] = uOutsideTessellation[1]; 
    }
}
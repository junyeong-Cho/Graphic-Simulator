#version 450
/**
 * \file params_quad.tesc
 * \author jiminlim
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

layout (vertices = 4) out;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

uniform vec4 uOutsideTessellation;
uniform vec2 uInsideTessellation;

void main() {
    
    gl_TessLevelOuter[0] = uOutsideTessellation.x;
    gl_TessLevelOuter[1] = uOutsideTessellation.y;
    gl_TessLevelOuter[2] = uOutsideTessellation.z;
    gl_TessLevelOuter[3] = uOutsideTessellation.w;
    
    gl_TessLevelInner[0] = uInsideTessellation.x;
    gl_TessLevelInner[1] = uInsideTessellation.y;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

}
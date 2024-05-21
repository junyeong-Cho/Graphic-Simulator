#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uProjection;
uniform float uTileScale;
uniform float uZ;

out vec3 vPos;

void main() {
    vPos = aPos * uTileScale;
    vPos.z = uZ;
    gl_Position = uProjection * vec4(aPos, 1.0);
}

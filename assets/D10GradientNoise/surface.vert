#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;
uniform float uTileScale;
uniform sampler2D uNoiseTexture;

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord * uTileScale;
    vec3 displacedPos = aPos + texture(uNoiseTexture, TexCoord).rgb;
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(displacedPos, 1.0);
}

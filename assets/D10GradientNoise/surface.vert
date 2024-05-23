#version 460

layout(location = 0) in vec3 aVertexPosition;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec2 TexCoord;

uniform sampler2D uTexture;
uniform float uTileScale;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjection;

void main()
{
    // Scale and interpolate texture coordinates
    TexCoord = aVertexTextureCoordinates * uTileScale;

    // Displace the vertex position based on the noise texture
    vec3 displacedPosition = aVertexPosition;
    displacedPosition.z += texture(uTexture, TexCoord).r;

    // Convert the vertex position to clip space
    gl_Position = uProjection * uViewMatrix * uModelMatrix * vec4(displacedPosition, 1.0);
}

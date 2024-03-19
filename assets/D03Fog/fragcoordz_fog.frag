#version 300 es
precision highp float;

in vec2 vTextureCoordinates;


layout(location = 0) out vec4 fFragmentColor;

uniform sampler2D uTex2d;
uniform vec3      uFogColor;
uniform float     uFogNear;
uniform float     uFogFar;

void main()
{
  vec4 color = texture(uTex2d, vTextureCoordinates);

  float fogAmount = smoothstep(uFogNear, uFogFar, gl_FragCoord.z);

  fFragmentColor = mix(color, vec4(uFogColor, 1.0), fogAmount);
}

#version 300 es
precision highp float;

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;
layout(location = 2) in vec2 aVertexTextureCoordinates;

out vec3 vColor;
out vec2 vTextureCoordinates;

uniform float uTime;

void main()
{
    // 시간에 따른 위치 변화를 계산
    float timeMod = mod(uTime, 6.0);                  // 시간을 2초 주기로 반복
    float bounceEffect = abs(sin(timeMod * 3.14159)); // 0과 1 사이를 반복하는 효과

    // 위치 변화를 적용하여 도형이 튕기는 효과를 줌
    vec3 modifiedPosition = aVertexPosition;
    modifiedPosition.x += bounceEffect * (aVertexPosition.x > 0.0 ? 1.0 : -1.0);
    modifiedPosition.y += bounceEffect * (aVertexPosition.y > 0.0 ? 1.0 : -1.0);

    // 스케일 변화 적용
    float interpolant = smoothstep(-1.0f, 1.0f, sin(uTime * 1.1f));
    float scale       = mix(0.3, 1.1, interpolant);
    gl_Position       = vec4(modifiedPosition * scale, 1.0);

    vColor = aVertexColor;
    vTextureCoordinates = aVertexTextureCoordinates;
}

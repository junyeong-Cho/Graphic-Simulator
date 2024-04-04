#version 300 es
precision       highp float;
precision highp sampler2DShadow;

layout(location = 0) out vec4 fFragmentColor;

in vec3 vNormalInViewSpace;
in vec3 vPositionInViewSpace;
in vec4 vPositionInShadowSpace;

uniform sampler2DShadow uShadowMap;
uniform vec3  uFogColor;
uniform float uFogDensity;
uniform vec3  uDiffuse;
uniform vec3  uAmbient;
uniform float uShininess;
uniform vec3  uSpecularColor;
uniform vec3  uLightPosition;
uniform bool  uDoShadowBehindLight;

void main() {
    float shadow;
    vec3 n = normalize(vNormalInViewSpace);
    vec3 l = normalize(uLightPosition - vPositionInViewSpace);
    float nl = max(dot(n, l), 0.0);
    const vec3 eye = vec3(0, 0, 1);
    vec3 h = normalize(l + eye);
    float spec = max(0.0, dot(n, h));
    spec = pow(spec, uShininess);
    vec3 diffuse = nl * uDiffuse;

    vec3 projCoords = vPositionInShadowSpace.xyz / vPositionInShadowSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // 적절한 깊이 비교 값을 포함하여 그림자 값을 계산합니다.
    if(!uDoShadowBehindLight && vPositionInShadowSpace.z < 0.0) {
        shadow = 1.0;
    } else {
        // texture 함수 호출 시 비교 깊이 값으로 projCoords.z 사용
        shadow = texture(uShadowMap, vec3(projCoords.xy, projCoords.z));
    }

    if(shadow < 0.5) { // 가정: shadow가 0.5 미만이면 그림자 안에 있는 것으로 간주
        fFragmentColor = vec4(uAmbient, 1.0);
        return;
    }

    vec3 color = uAmbient + shadow * (diffuse + spec * uSpecularColor);

    float fogAmount = clamp(uFogDensity * length(vPositionInViewSpace), 0.0, 1.0);
    fFragmentColor = vec4(mix(color, uFogColor, fogAmount), 1.0);
}

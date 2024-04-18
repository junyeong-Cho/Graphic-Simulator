#version 300 es
layout(triangles) in;
layout(triangle_strip, max_vertices = 14) out;

in vec2 vTextureCoordinates[];
in vec3 vNormals[];

out vec2 fTextureCoordinates;
out float fLightIntensity;

uniform mat4 uModelMatrix;
uniform float uExtrudeFactor;
uniform bool uFlat;

void main() {
    vec3 normal;
    if (uFlat) {
        // 평면 셰이딩에서 사용되는 법선
        vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        normal = normalize(cross(edge1, edge2));
    } else {
        // 버텍스 정상에 기반한 평활 셰이딩
        normal = normalize((vNormals[0] + vNormals[1] + vNormals[2]) / 3.0);
    }

    // 기본 삼각형 복제
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0; // 기본 위치, 색상은 검은색
        EmitVertex();
    }
    EndPrimitive();

    // 측면 삼각형 생성
    for (int i = 0; i < 3; ++i) {
        int next = (i + 1) % 3;

        // 원래 위치
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0;
        EmitVertex();

        // extrude 위치
        gl_Position = gl_in[i].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0; // extrude 위치, 색상은 밝은색
        EmitVertex();

        // 다음 원래 위치
        gl_Position = gl_in[next].gl_Position;
        fTextureCoordinates = vTextureCoordinates[next];
        fLightIntensity = 0.0;
        EmitVertex();

        // 다음 extrude 위치
        gl_Position = gl_in[next].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[next];
        fLightIntensity = 1.0;
        EmitVertex();

        EndPrimitive();
    }

    // 윗면 삼각형
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0;
        EmitVertex();
    }
    EndPrimitive();
}

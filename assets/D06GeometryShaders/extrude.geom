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

void main() 
{
    vec3 normal;
    if (uFlat) 
    {
        vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        normal = normalize(cross(edge1, edge2));
    } 
    else 
    {
        normal = normalize((vNormals[0] + vNormals[1] + vNormals[2]) / 3.0);
    }

    // 원본 삼각형 출력
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0;
        EmitVertex();
    }
    EndPrimitive();

    // 압출하여 만든 사이드 페이스
    for (int i = 0; i < 3; ++i) 
    {
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0;
        EmitVertex();

        // EndPrimitive() 호출을 적절한 위치에 추가
        if (i < 2) {
            EndPrimitive();
            EmitVertex();  // 시작점 재발생
        }
    }
    EndPrimitive();  // 루프 종료 후 마지막 프리미티브 종료

    // 상단 삼각형
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0;
        EmitVertex();
    }
    EndPrimitive();
}

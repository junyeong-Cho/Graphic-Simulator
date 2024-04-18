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
        // ��� ���̵����� ���Ǵ� ����
        vec3 edge1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 edge2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        normal = normalize(cross(edge1, edge2));
    } else {
        // ���ؽ� ���� ����� ��Ȱ ���̵�
        normal = normalize((vNormals[0] + vNormals[1] + vNormals[2]) / 3.0);
    }

    // �⺻ �ﰢ�� ����
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0; // �⺻ ��ġ, ������ ������
        EmitVertex();
    }
    EndPrimitive();

    // ���� �ﰢ�� ����
    for (int i = 0; i < 3; ++i) {
        int next = (i + 1) % 3;

        // ���� ��ġ
        gl_Position = gl_in[i].gl_Position;
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 0.0;
        EmitVertex();

        // extrude ��ġ
        gl_Position = gl_in[i].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0; // extrude ��ġ, ������ ������
        EmitVertex();

        // ���� ���� ��ġ
        gl_Position = gl_in[next].gl_Position;
        fTextureCoordinates = vTextureCoordinates[next];
        fLightIntensity = 0.0;
        EmitVertex();

        // ���� extrude ��ġ
        gl_Position = gl_in[next].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[next];
        fLightIntensity = 1.0;
        EmitVertex();

        EndPrimitive();
    }

    // ���� �ﰢ��
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position + vec4(normal * uExtrudeFactor, 0.0);
        fTextureCoordinates = vTextureCoordinates[i];
        fLightIntensity = 1.0;
        EmitVertex();
    }
    EndPrimitive();
}

#version 330 core

layout (location = 0) in vec3 vPos;    // ���� ��ġ
layout (location = 1) in vec3 vColor;  // ���� ����
layout (location = 2) in vec3 vNormal; // ���� ����

out vec3 FragPos;   // ���� ��ǥ�迡���� ���� ��ġ
out vec3 Normal;    // ��ȯ�� ���� ����
out vec3 VertexColor; // ���ؽ� ����

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    FragPos = vec3(model * vec4(vPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vNormal;
    VertexColor = vColor; // ���� ���� ����
}

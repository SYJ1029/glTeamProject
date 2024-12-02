#version 330 core

layout (location = 0) in vec3 vPos;    // 정점 위치
layout (location = 1) in vec3 vColor;  // 정점 색상
layout (location = 2) in vec3 vNormal; // 법선 벡터

out vec3 FragPos;   // 월드 좌표계에서의 정점 위치
out vec3 Normal;    // 변환된 법선 벡터
out vec3 VertexColor; // 버텍스 색상

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vPos, 1.0);
    FragPos = vec3(model * vec4(vPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vNormal;
    VertexColor = vColor; // 정점 색상 전달
}

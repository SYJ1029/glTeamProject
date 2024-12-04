#version 330 core

in vec3 FragPos;        // ���� ��ǥ�迡���� �ȼ� ��ġ
in vec3 Normal;         // ��ȯ�� ���� ����
in vec3 VertexColor;    // ���ؽ����� ���޵� ����

out vec4 FragColor;

uniform vec3 lightPos;   // ���� ��ǥ������ ���� ��ġ
uniform vec3 lightColor; // ���� ����
uniform vec3 viewPos;    // ���� ��ǥ������ ī�޶� ��ġ
uniform float ambientLight; // �ֺ���
uniform vec3 objectColor;   // ����

void main() {
    // Ambient lighting
    vec3 ambient = ambientLight * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    int shininess = 128;
    float specularStrength = 0.8;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float specularLight = max (dot (viewDir, reflectDir), 0.0);
    specularLight = pow(specularLight, shininess);
    vec3 specular = specularStrength * specularLight * lightColor;

    vec3 finalColor = (objectColor != vec3(0.0, 0.0, 0.0)) ? objectColor : VertexColor;

    // Combine lighting components
    vec3 result = (ambient + diffuse + specular) * finalColor;
    FragColor = vec4(result, 1.0);
}

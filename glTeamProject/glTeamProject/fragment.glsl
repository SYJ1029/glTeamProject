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
uniform float buildalpha;
uniform int objtype;

uniform float fogStart;  // �Ȱ��� ���۵Ǵ� �Ÿ�
uniform float fogEnd;    // �Ȱ��� ������ £������ �Ÿ�
uniform vec3 fogColor;   // �Ȱ� ����

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
    float specularComponent = max(dot(viewDir, reflectDir), 0.0);
    specularComponent = pow(specularComponent, shininess);
    vec3 specular = specularStrength * specularComponent * lightColor;

    vec3 finalColor = (objectColor != vec3(0.0, 0.0, 0.0)) ? objectColor : VertexColor;
    vec3 result = (ambient + diffuse + specular) * finalColor;

    // Compute fog effect based on distance
    float dist = length(viewPos - FragPos);
    float fogFactor = (dist - fogStart) / (fogEnd - fogStart);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Combine the fog effect with the final color
    vec3 finalResult = mix(result, fogColor, fogFactor);

    if(objtype == 2){
        FragColor = vec4(finalResult, buildalpha);
    }
    else{
        FragColor = vec4(finalResult, 1.0);
    }
}

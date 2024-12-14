#version 330 core

in vec3 FragPos;        // 월드 좌표계에서의 픽셀 위치
in vec3 Normal;         // 변환된 법선 벡터
in vec3 VertexColor;    // 버텍스에서 전달된 색상

out vec4 FragColor;

uniform vec3 lightPos;   // 월드 좌표에서의 조명 위치
uniform vec3 lightColor; // 조명 색상
uniform vec3 viewPos;    // 월드 좌표에서의 카메라 위치
uniform float ambientLight; // 주변광
uniform vec3 objectColor;   // 색상값
uniform float buildalpha;
uniform int objtype;

uniform float fogStart;  // 안개가 시작되는 거리
uniform float fogEnd;    // 안개가 완전히 짙어지는 거리
uniform vec3 fogColor;   // 안개 색상

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

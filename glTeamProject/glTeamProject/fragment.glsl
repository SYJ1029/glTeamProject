#version 330 core

out vec4 FragColor;

uniform vec3 objectColor; // ¸ðµ¨ÀÇ »ö»ó

void main() {
    FragColor = vec4(objectColor, 1.0); // Alpha = 1.0
}

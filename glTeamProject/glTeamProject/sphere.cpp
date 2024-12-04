#include "sphere.h"

GLuint sphereVAO, sphereVBO;
int indexCount = 0;

void initSphereBuffer(float radius, int stackCount, int sectorCount) {
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	// 위도와 경도를 기반으로 정점 좌표 계산
	for (int i = 0; i <= stackCount; ++i) {
		float stackAngle = M_PI / 2 - i * (M_PI / stackCount); // -pi/2 ~ pi/2
		float xy = radius * cos(stackAngle); // x, y 반지름
		float z = radius * sin(stackAngle);  // z 좌표

		for (int j = 0; j <= sectorCount; ++j) {
			float sectorAngle = j * (2 * M_PI / sectorCount); // 0 ~ 2pi

			// 정점의 위치
			float x = xy * cos(sectorAngle);
			float y = xy * sin(sectorAngle);

			// 정점 추가 (위치)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// 정점 추가 (색상)
			vertices.push_back((x + 1.0f) / 2.0f); // R (0.0 ~ 1.0)
			vertices.push_back((y + 1.0f) / 2.0f); // G (0.0 ~ 1.0)
			vertices.push_back((z + 1.0f) / 2.0f); // B (0.0 ~ 1.0)

			// 정점 추가 (법선)
			vertices.push_back(x / radius);
			vertices.push_back(y / radius);
			vertices.push_back(z / radius);
		}
	}

	// 인덱스 계산
	for (int i = 0; i < stackCount; ++i) {
		int stackStart = i * (sectorCount + 1);
		int nextStackStart = (i + 1) * (sectorCount + 1);

		for (int j = 0; j < sectorCount; ++j) {
			indices.push_back(stackStart + j);
			indices.push_back(nextStackStart + j);
			indices.push_back(stackStart + j + 1);

			indices.push_back(stackStart + j + 1);
			indices.push_back(nextStackStart + j);
			indices.push_back(nextStackStart + j + 1);
		}
	}
	indexCount = indices.size();

	// VAO 생성 및 바인딩
	GLuint sphereEBO;
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// EBO 생성 및 데이터 전송
	glGenBuffers(1, &sphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// 위치 속성 (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// 색상 속성 (attribute 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// 법선 속성 (attribute 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// VAO 언바인딩
	glBindVertexArray(0);
}
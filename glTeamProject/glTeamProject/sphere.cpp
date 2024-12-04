#include "sphere.h"

GLuint sphereVAO, sphereVBO;
int indexCount = 0;

void initSphereBuffer(float radius, int stackCount, int sectorCount) {
	std::vector<GLfloat> vertices;
	std::vector<GLuint> indices;

	// ������ �浵�� ������� ���� ��ǥ ���
	for (int i = 0; i <= stackCount; ++i) {
		float stackAngle = M_PI / 2 - i * (M_PI / stackCount); // -pi/2 ~ pi/2
		float xy = radius * cos(stackAngle); // x, y ������
		float z = radius * sin(stackAngle);  // z ��ǥ

		for (int j = 0; j <= sectorCount; ++j) {
			float sectorAngle = j * (2 * M_PI / sectorCount); // 0 ~ 2pi

			// ������ ��ġ
			float x = xy * cos(sectorAngle);
			float y = xy * sin(sectorAngle);

			// ���� �߰� (��ġ)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);

			// ���� �߰� (����)
			vertices.push_back((x + 1.0f) / 2.0f); // R (0.0 ~ 1.0)
			vertices.push_back((y + 1.0f) / 2.0f); // G (0.0 ~ 1.0)
			vertices.push_back((z + 1.0f) / 2.0f); // B (0.0 ~ 1.0)

			// ���� �߰� (����)
			vertices.push_back(x / radius);
			vertices.push_back(y / radius);
			vertices.push_back(z / radius);
		}
	}

	// �ε��� ���
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

	// VAO ���� �� ���ε�
	GLuint sphereEBO;
	glGenVertexArrays(1, &sphereVAO);
	glBindVertexArray(sphereVAO);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &sphereVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	// EBO ���� �� ������ ����
	glGenBuffers(1, &sphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	// ��ġ �Ӽ� (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// ���� �Ӽ� (attribute 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// ���� �Ӽ� (attribute 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// VAO ����ε�
	glBindVertexArray(0);
}
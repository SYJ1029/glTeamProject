#include "floor.h"

GLuint floorVAO, floorVBO;

void InitFloor() {
	// �ٴ� ���� ������
	GLfloat floorVertices[] = {
		// Positions          // Colors
		-50.0f, 0.0f, -50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0.0f,  50.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	};

	// �ٴ��� �ε��� ������
	GLuint floorIndices[] = {
		0, 1, 2, // ù ��° �ﰢ��
		0, 3, 2  // �� ��° �ﰢ��
	};

	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) ���� �� ������ ����
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

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

void drawFloor(GLint modelLoc, Player &player) {
	// �ٴ�
	glBindVertexArray(floorVAO);
	mat4 floorModel = mat4(1.0f); // �� ���
	floorModel = translate(floorModel, (vec3(player.x, 0.0f, player.z)));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(floorModel));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

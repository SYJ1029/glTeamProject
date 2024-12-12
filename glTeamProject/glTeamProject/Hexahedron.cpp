#include "Hexahedron.h"

GLuint hexVao, hexVbo;

void Initbuffer() {
	GLfloat vertices[] = {
		// ��ǥ               // ����         // �븻 (nx, ny, nz)
		// �޸�
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		// �ո�
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

		// ���� ��
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,

		// ������ ��
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,

		 // �Ʒ���
		 -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		 -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		 // ����
		 -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		  0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		  0.5f, 0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 -0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	};

	GLuint indices[] = {
		// �޸�
		0, 1, 2,   // ù ��° �ﰢ��
		0, 2, 3,   // �� ��° �ﰢ��

		// �ո�
		4, 5, 6,   // ù ��° �ﰢ��
		4, 6, 7,   // �� ��° �ﰢ��

		// ���� ��
		8, 9, 10,  // ù ��° �ﰢ��
		8, 10, 11, // �� ��° �ﰢ��

		// ������ ��
		12, 13, 14, // ù ��° �ﰢ��
		12, 14, 15, // �� ��° �ﰢ��

		// �Ʒ��� ��
		16, 17, 18, // ù ��° �ﰢ��
		16, 18, 19, // �� ��° �ﰢ��

		// ���� ��
		20, 21, 22, // ù ��° �ﰢ��
		20, 22, 23  // �� ��° �ﰢ��
	};

	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &hexVao);
	glBindVertexArray(hexVao);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &hexVbo);
	glBindBuffer(GL_ARRAY_BUFFER, hexVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) ���� �� ������ ����
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
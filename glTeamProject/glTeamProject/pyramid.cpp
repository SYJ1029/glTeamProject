#include "pyramid.h"

GLuint pyramidVao, pyramidVbo;

void InitPyramidBuffer() {
	GLfloat vertices[] = {
		// �Ʒ��� (4���� ����)
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// �ո� (3���� ����)
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.447f, -0.894f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.447f, -0.894f,
		 0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.447f, -0.894f,

		 // ������ �� (3���� ����)
		  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.894f, 0.447f, 0.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.894f, 0.447f, 0.0f,
		  0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, 0.894f, 0.447f, 0.0f,

		  // �޸� (3���� ����)
		   0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.447f, 0.894f,
		  -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.447f, 0.894f,
		   0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.447f, 0.894f,

		   // ���� �� (3���� ����)
		   -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, -0.894f, 0.447f, 0.0f,
		   -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.894f, 0.447f, 0.0f,
			0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, -0.894f, 0.447f, 0.0f,
	};

	GLuint indices[] = {
		// �Ʒ���
		0, 1, 2,
		0, 2, 3,
		// �ո�
		4, 5, 6,
		// ������ ��
		7, 8, 9,
		// �޸�
		10, 11, 12,
		// ���� ��
		13, 14, 15,
	};


	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &pyramidVao);
	glBindVertexArray(pyramidVao);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &pyramidVbo);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO ���� �� ������ ����
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

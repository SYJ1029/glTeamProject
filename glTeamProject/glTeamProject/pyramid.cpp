#include "pyramid.h"

GLuint pyramidVao, pyramidVbo;

void InitPyramidBuffer() {
	GLfloat vertices[] = {
		// 아래면 (4개의 정점)
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		// 앞면 (3개의 정점)
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.447f, -0.894f,
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.447f, -0.894f,
		 0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.447f, -0.894f,

		 // 오른쪽 면 (3개의 정점)
		  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.894f, 0.447f, 0.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.894f, 0.447f, 0.0f,
		  0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, 0.894f, 0.447f, 0.0f,

		  // 뒷면 (3개의 정점)
		   0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.447f, 0.894f,
		  -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.447f, 0.894f,
		   0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.447f, 0.894f,

		   // 왼쪽 면 (3개의 정점)
		   -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, -0.894f, 0.447f, 0.0f,
		   -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -0.894f, 0.447f, 0.0f,
			0.0f,  0.8f,  0.0f, 1.0f, 1.0f, 1.0f, -0.894f, 0.447f, 0.0f,
	};

	GLuint indices[] = {
		// 아래면
		0, 1, 2,
		0, 2, 3,
		// 앞면
		4, 5, 6,
		// 오른쪽 면
		7, 8, 9,
		// 뒷면
		10, 11, 12,
		// 왼쪽 면
		13, 14, 15,
	};


	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &pyramidVao);
	glBindVertexArray(pyramidVao);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &pyramidVbo);
	glBindBuffer(GL_ARRAY_BUFFER, pyramidVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO 생성 및 데이터 전송
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

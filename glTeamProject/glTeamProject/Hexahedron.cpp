#include "Hexahedron.h"

GLuint hexVao, hexVbo;

void Initbuffer() {
	GLfloat vertices[] = {
		// 좌표               // 색상         // 노말 (nx, ny, nz)
		// 뒷면
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		// 앞면
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

		// 왼쪽 면
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,

		// 오른쪽 면
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,

		 // 아래면
		 -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		  0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
		 -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

		 // 윗면
		 -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		  0.5f, 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		  0.5f, 0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		 -0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	};

	GLuint indices[] = {
		// 뒷면
		0, 1, 2,   // 첫 번째 삼각형
		0, 2, 3,   // 두 번째 삼각형

		// 앞면
		4, 5, 6,   // 첫 번째 삼각형
		4, 6, 7,   // 두 번째 삼각형

		// 왼쪽 면
		8, 9, 10,  // 첫 번째 삼각형
		8, 10, 11, // 두 번째 삼각형

		// 오른쪽 면
		12, 13, 14, // 첫 번째 삼각형
		12, 14, 15, // 두 번째 삼각형

		// 아래쪽 면
		16, 17, 18, // 첫 번째 삼각형
		16, 18, 19, // 두 번째 삼각형

		// 위쪽 면
		20, 21, 22, // 첫 번째 삼각형
		20, 22, 23  // 두 번째 삼각형
	};

	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &hexVao);
	glBindVertexArray(hexVao);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &hexVbo);
	glBindBuffer(GL_ARRAY_BUFFER, hexVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) 생성 및 데이터 전송
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
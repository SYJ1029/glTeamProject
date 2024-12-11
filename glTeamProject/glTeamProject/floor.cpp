#include "floor.h"

GLuint floorVAO, floorVBO;

void InitFloor() {
	// 바닥 정점 데이터
	GLfloat floorVertices[] = {
		// Positions          // Colors
		-50.0f, 0.0f, -50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0.0f,  50.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	};

	// 바닥의 인덱스 데이터
	GLuint floorIndices[] = {
		0, 1, 2, // 첫 번째 삼각형
		0, 3, 2  // 두 번째 삼각형
	};

	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) 생성 및 데이터 전송
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

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

void drawFloor(GLint modelLoc, Player &player) {
	// 바닥
	glBindVertexArray(floorVAO);
	mat4 floorModel = mat4(1.0f); // 모델 행렬
	floorModel = translate(floorModel, (vec3(player.x, 0.0f, player.z)));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(floorModel));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

#pragma once

#include "Enemy.h"
#include "randoms.h"


void InitEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies) {
	Enemy newenemy; // 새로 만들 적 선언
	int enemyseed = 100; // seed 값을 지정


	newenemy.x = GetRandomNumber(enemyseed);
	newenemy.y = 0.0f;
	newenemy.z = GetRandomNumber(enemyseed); // seed에 따라 좌표를 배정
	newenemy.angleX = 0, newenemy.angleY = 0, newenemy.angleZ = 0;	// 생성 당시에는 player를 바라보지는 않는다.

	newenemy.x += playerx;
	newenemy.z += playerz;

	newenemy.speed = 0.01f;

	g_enemies.push_back(newenemy); // 리스트에 추가
}


void MoveEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies) {
	float dx = 0, dz = 0;
	for (int i = 0; i < g_enemies.size(); i++) {
		dx = playerx - g_enemies[i].x;
		dz = playerz - g_enemies[i].z;

		g_enemies[i].x = g_enemies[i].x + dx * g_enemies[i].speed;
		g_enemies[i].z = g_enemies[i].z + dz * g_enemies[i].speed;
	}
}

void drawEnemy(GLint modelLoc, GLUquadricObj*& qobj, std::vector<Enemy>& g_enemies) {
	glBindVertexArray(sphereVAO);

	mat4 baseModelMat = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)); // 적이 그려지는 도형 전체에 대한 이동


	for (int i = 0; i < g_enemies.size(); i++) {
		baseModelMat = glm::translate(mat4(1.0f), vec3(g_enemies[i].x, g_enemies[i].y, g_enemies[i].z));

		mat4 enemyModelMat = mat4(1.0f); // 적 모델 행렬
		glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.5f, 1.0f);

		enemyModelMat *= baseModelMat;
		enemyModelMat = glm::translate(enemyModelMat, vec3(0.0f, 2.7f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(enemyModelMat));
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

		enemyModelMat = baseModelMat;
		enemyModelMat = glm::rotate(enemyModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		enemyModelMat = glm::translate(enemyModelMat, vec3(0.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(enemyModelMat));
		gluCylinder(qobj, 1.0f, 1.0f, 2.0f, 20.0f, 8.0f);

		enemyModelMat = glm::translate(enemyModelMat, vec3(0.0f, 0.0f, 0.0f));
		enemyModelMat *= baseModelMat; // 회전한 흔적 제거
	}


	//glBindVertexArray(enemyVAO);
	glBindVertexArray(0); // VAO 언바인딩
}
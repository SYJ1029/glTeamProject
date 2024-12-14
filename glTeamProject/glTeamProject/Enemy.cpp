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

typedef struct Direction {
	int x;
	int z;
};

vec3 AStar(float playerx, float playerz, Enemy& enemy, int** maptile, int row, int column) {
	// 0. 타일 상의 이동방향 정의


	Direction ways[8] = {
		{-1, 1}, {0, 1}, {1, 1},
		{-1, 0}, {1, 0},
		{-1, -1}, {0, -1}, {1, -1}
	};
	// 1. Enemy와 Player의 타일상의 위치를 받아오기

	int i = (int)((enemy.x + 100.0f) / 5.0f);
	int j = (int)((enemy.z + 100.0f) / 5.0f);

	int pi = (int)((playerx + 100.0f) / 5.0f);
	int pj = (int)((playerz + 100.0f) / 5.0f);

	// 2. 직선거리방향 타일 정의
	int dx = normalize(vec3(playerx - enemy.x, 0.0f, 0.0f)).x;
	int dz = normalize(vec3(0.0f, 0.0f, playerz - enemy.z)).z;

	// 3. 최단거리방향 타일이 비어있는지 검사
	
	if (maptile[i][j] == 0)
		return vec3(dx, 0, dz); // 비어있다면, 그 타일을 향해 이동한다.
	
	// 4. 나머지 방향들 중에서 거리가 가장 적은 타일을 찾는다.

	float maxdist = 0;
	Direction result = { -5, -5 };
	for (int index = 0; index < 8; index++) {
		Direction way = ways[index];

		if (i + way.x < 0 || i + way.x >= row ||
			j + way.z < 0 || j + way.z >= column)
			continue; // 범위를 벗어난 값은 고려하지 않는다.

		if (distance(vec3(index + way.x, 0.0f, index + way.z), vec3(pj, 0.0f, pi)) > maxdist) {
			maxdist = distance(vec3(index + way.x, 0.0f, index + way.z), vec3(pj, 0.0f, pi));
			result = way;
		}

	}
	
	if (result.z < -1) return vec3(0, 0, 0); // 잘못된 값이 나왔다면 일단 정지시킨다.

	return normalize(vec3(i + result.x, 0.0f, j + result.z)); // 계산 결과를 정규화 하여 반환
}

void MoveEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies, int** maptile, int row, int column) {
	float dx = 0, dz = 0;

	for (int i = 0; i < g_enemies.size(); i++) {
		dx = playerx - g_enemies[i].x;
		dz = playerz - g_enemies[i].z;

		vec3 direct = AStar(playerx, playerz, g_enemies[i], maptile, row, column);
		g_enemies[i].x = g_enemies[i].x + direct.x * g_enemies[i].speed;
		g_enemies[i].z = g_enemies[i].z + direct.z * g_enemies[i].speed;
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
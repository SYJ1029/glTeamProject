#pragma once

#include "Enemy.h"
#include "randoms.h"
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

void InitEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies) {
	Enemy newenemy; // 새로 만들 적 선언
	int enemyseed = 100; // seed 값을 지정


	newenemy.x = GetRandomNumber(enemyseed);
	newenemy.y = 0.0f;
	newenemy.z = GetRandomNumber(enemyseed); // seed에 따라 좌표를 배정
	newenemy.angleX = 0, newenemy.angleY = 0, newenemy.angleZ = 0;	// 생성 당시에는 player를 바라보지는 않는다.

	newenemy.x += playerx * 2;
	newenemy.z += playerz * 2; // 적의 좌표가 player로부터 되도록이면 멀도록

	newenemy.speed = 0.01f;
	newenemy.hp = 10;
	newenemy.damage = 5;
	newenemy.damaged = false;

	newenemy.backframe = 0;
	newenemy.died = false;
	newenemy.first = true;

	g_enemies.push_back(newenemy); // 리스트에 추가
}

typedef struct Direction {
	int x;
	int z;
};

int backcnt = 0;

vec3 AStar(float playerx, float playerz, Enemy& enemy, int** maptile, int row, int column) {
	// 0. 타일 상의 이동방향 정의


	Direction ways[8] = {
		{-1, 1}, {0, 1}, {1, 1},
		{-1, 0}, {1, 0},
		{-1, -1}, {0, -1}, {1, -1}
	};

	// 0. 직선거리방향 타일 정의
	int dx = vec3(playerx - enemy.x, 0.0f, 0.0f).x;
	int dz = vec3(0.0f, 0.0f, playerz - enemy.z).z;

	//if (dx != 0) dx /= dx;
	//if (dz != 0) dz /= dz;
	// 1. Enemy와 Player의 타일상의 위치를 받아오기


	int i = (int)((enemy.x + 100.0f) / 5.0f);
	int j = (int)((enemy.z + 100.0f) / 5.0f);


	int pi = (int)((playerx + 100.0f) / 5.0f);
	int pj = (int)((playerz + 100.0f) / 5.0f);

	if (dx != 0)
		dx /= abs(dx);
	if (dz != 0)
		dz /= abs(dz);

	// 2. 최단거리방향 타일이 비어있는지 검사

	if (maptile[i + dx][j + dz] == 0)
		return vec3(dx * 10, 0, dz * 10); // 비어있다면, 그 타일을 향해 이동한다.

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

	if (result.z < -1)
		return vec3(0, 0, 0); // 잘못된 값이 나왔다면 일단 정지시킨다.

	i += result.x;
	j += result.z;





	return vec3(i, 0.0f, j); // 계산 결과를 반환
}

void EnemyDamageFunc(int value) {

}

void MoveEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies, int** maptile, int row, int column) {
	float dx = 0, dz = 0;
	vec3 direct;

	for (int i = 0; i < g_enemies.size(); i++) {

		if (g_enemies[i].hp > 0 && g_enemies[i].died == false) { // 적이 살아있을 때만 진입
			if (g_enemies[i].damaged) {
				dx = playerx - g_enemies[i].x;
				dz = playerz - g_enemies[i].z;

				direct = { -dx * 3.0f, 0.0f, -dz * 3.0f };


				g_enemies[i].backframe++;

				if (g_enemies[i].backframe > 3) {
					g_enemies[i].backframe = 0;
					g_enemies[i].damaged = false;
				}
			}
			else {
				dx = playerx - g_enemies[i].x;
				dz = playerz - g_enemies[i].z;

				direct = AStar(playerx, playerz, g_enemies[i], maptile, row, column);
			}

			g_enemies[i].x = g_enemies[i].x + direct.x * g_enemies[i].speed;
			g_enemies[i].z = g_enemies[i].z + direct.z * g_enemies[i].speed;
		}
		else {

			if (g_enemies[i].first) {
				glutTimerFunc(10, EnemyDyingFunc, i);
				g_enemies[i].first = false;
			}
			
		}
	}
}



void drawEnemy(GLint modelLoc, GLUquadricObj*& qobj, std::vector<Enemy>& g_enemies) {
	glBindVertexArray(sphereVAO);

	mat4 baseModelMat = mat4(1.0f);
	baseModelMat = glm::translate(baseModelMat, vec3(0.0f, 0.0f, 0.0f)); // 적이 그려지는 도형 전체에 대한 이동


	for (int i = 0; i < g_enemies.size(); i++) {
		baseModelMat = glm::rotate(baseModelMat, glm::radians(g_enemies[i].angleX), vec3(1.0f, 0.0f, 0.0f));

		baseModelMat = glm::translate(baseModelMat, vec3(g_enemies[i].x, g_enemies[i].y, g_enemies[i].z));
		baseModelMat = glm::rotate(baseModelMat, glm::radians(g_enemies[i].angleZ), vec3(0.0f, 0.0f, 1.0f));
		baseModelMat = glm::translate(baseModelMat, vec3(-g_enemies[i].x, -g_enemies[i].y, -g_enemies[i].z));

		baseModelMat = glm::translate(baseModelMat, vec3(g_enemies[i].x, g_enemies[i].y, g_enemies[i].z));

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

		baseModelMat = mat4(1.0f);
		baseModelMat = glm::translate(baseModelMat, vec3(0.0f, 0.0f, 0.0f)); // 다시 초기화
	}


	//glBindVertexArray(enemyVAO);
	glBindVertexArray(0); // VAO 언바인딩
}

void EnemyDyingFunc(int value) {
	g_enemies[value].angleZ += 1;



	if (g_enemies[value].angleZ >= 90.0f) {
		g_enemies.erase(g_enemies.begin() + value);
	}
	else
		glutTimerFunc(10, EnemyDyingFunc, value);
}
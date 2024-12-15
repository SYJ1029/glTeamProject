#pragma once

#include "Enemy.h"
#include "randoms.h"
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

void InitEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies) {
	Enemy newenemy; // ���� ���� �� ����
	int enemyseed = 100; // seed ���� ����


	newenemy.x = GetRandomNumber(enemyseed);
	newenemy.y = 0.0f;
	newenemy.z = GetRandomNumber(enemyseed); // seed�� ���� ��ǥ�� ����
	newenemy.angleX = 0, newenemy.angleY = 0, newenemy.angleZ = 0;	// ���� ��ÿ��� player�� �ٶ����� �ʴ´�.

	newenemy.x += playerx;
	newenemy.z += playerz;

	newenemy.speed = 0.01f;
	newenemy.hp = 10;
	newenemy.damage = 5;
	newenemy.damaged = false;

	newenemy.backframe = 0;
	newenemy.died = false;
	newenemy.first = true;

	g_enemies.push_back(newenemy); // ����Ʈ�� �߰�
}

typedef struct Direction {
	int x;
	int z;
};

int backcnt = 0;

vec3 AStar(float playerx, float playerz, Enemy& enemy, int** maptile, int row, int column) {
	// 0. Ÿ�� ���� �̵����� ����


	Direction ways[8] = {
		{-1, 1}, {0, 1}, {1, 1},
		{-1, 0}, {1, 0},
		{-1, -1}, {0, -1}, {1, -1}
	};
	// 1. Enemy�� Player�� Ÿ�ϻ��� ��ġ�� �޾ƿ���

	std::vector<Direction> tilelist;

	int i = (int)((enemy.x + 100.0f) / 5.0f);
	int j = (int)((enemy.z + 100.0f) / 5.0f);

	tilelist.push_back({ i, j });

	for (int count = 0; count < 4; count++) {
		int pi = (int)((playerx + 100.0f) / 5.0f);
		int pj = (int)((playerz + 100.0f) / 5.0f);

		// 2. �����Ÿ����� Ÿ�� ����
		int dx = vec3(playerx - enemy.x, 0.0f, 0.0f).x;
		int dz = vec3(0.0f, 0.0f, playerz - enemy.z).z;

		// 3. �ִܰŸ����� Ÿ���� ����ִ��� �˻�

		if (maptile[tilelist[count].x][tilelist[count].z] == 0)
			return vec3(dx, 0, dz); // ����ִٸ�, �� Ÿ���� ���� �̵��Ѵ�.

		// 4. ������ ����� �߿��� �Ÿ��� ���� ���� Ÿ���� ã�´�.

		float maxdist = 0;
		Direction result = { -5, -5 };

		for (int index = 0; index < 8; index++) {
			Direction way = ways[index];

			if (tilelist[count].x + way.x < 0 || tilelist[count].x + way.x >= row ||
				tilelist[count].z + way.z < 0 || tilelist[count].z + way.z >= column)
				continue; // ������ ��� ���� ������� �ʴ´�.

			if (distance(vec3(index + way.x, 0.0f, index + way.z), vec3(pj, 0.0f, pi)) > maxdist) {
				maxdist = distance(vec3(index + way.x, 0.0f, index + way.z), vec3(pj, 0.0f, pi));
				result = way;
			}

		}

		if (result.z < -1) 
			return vec3(0, 0, 0); // �߸��� ���� ���Դٸ� �ϴ� ������Ų��.

		i += result.x;
		j += result.z;

		tilelist.push_back({ i, j });
	}

	

	return vec3(tilelist[1].x, 0.0f, tilelist[1].z); // ��� ����� ��ȯ
}

void EnemyDamageFunc(int value) {

}

void MoveEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies, int** maptile, int row, int column) {
	float dx = 0, dz = 0;
	vec3 direct;

	for (int i = 0; i < g_enemies.size(); i++) {

		if (g_enemies[i].hp > 0 && g_enemies[i].died == false) { // ���� ������� ���� ����
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
	baseModelMat = glm::translate(baseModelMat, vec3(0.0f, 0.0f, 0.0f)); // ���� �׷����� ���� ��ü�� ���� �̵�


	for (int i = 0; i < g_enemies.size(); i++) {
		baseModelMat = glm::rotate(baseModelMat, glm::radians(g_enemies[i].angleX), vec3(1.0f, 0.0f, 0.0f));

		baseModelMat = glm::translate(baseModelMat, vec3(g_enemies[i].x, g_enemies[i].y, g_enemies[i].z));
		baseModelMat = glm::rotate(baseModelMat, glm::radians(g_enemies[i].angleZ), vec3(0.0f, 0.0f, 1.0f));
		baseModelMat = glm::translate(baseModelMat, vec3(-g_enemies[i].x, -g_enemies[i].y, -g_enemies[i].z));

		baseModelMat = glm::translate(baseModelMat, vec3(g_enemies[i].x, g_enemies[i].y, g_enemies[i].z));

		mat4 enemyModelMat = mat4(1.0f); // �� �� ���
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
		enemyModelMat *= baseModelMat; // ȸ���� ���� ����

		baseModelMat = mat4(1.0f);
		baseModelMat = glm::translate(baseModelMat, vec3(0.0f, 0.0f, 0.0f)); // �ٽ� �ʱ�ȭ
	}


	//glBindVertexArray(enemyVAO);
	glBindVertexArray(0); // VAO ����ε�
}

void EnemyDyingFunc(int value) {
	g_enemies[value].angleZ += 1;



	if (g_enemies[value].angleZ >= 90.0f) {
		g_enemies.erase(g_enemies.begin() + value);
	}
	else
		glutTimerFunc(10, EnemyDyingFunc, value);
}
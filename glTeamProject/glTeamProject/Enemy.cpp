#pragma once

#include "Enemy.h"
#include "randoms.h"


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

	g_enemies.push_back(newenemy); // ����Ʈ�� �߰�
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

	mat4 baseModelMat = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)); // ���� �׷����� ���� ��ü�� ���� �̵�


	for (int i = 0; i < g_enemies.size(); i++) {
		baseModelMat = glm::translate(mat4(1.0f), vec3(g_enemies[i].x, g_enemies[i].y, g_enemies[i].z));

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
	}


	//glBindVertexArray(enemyVAO);
	glBindVertexArray(0); // VAO ����ε�
}
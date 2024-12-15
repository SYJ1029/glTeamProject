#pragma once

#include "sphere.h"
#include "shader.h"
#include <gl/glew.h>
#include <gl/glew.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

using namespace glm;

typedef struct Enemy {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	GLfloat angleX, angleY, angleZ;
	GLfloat speed;
	GLint hp;
	GLint damage;
	bool damaged;

	int backframe;
};

//extern GLuint enemyVAO, enemyVBO, enemyEBO;
extern std::vector<Enemy> g_enemies;

void InitEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies);
void MoveEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies, int** maptile, int row, int column);
void drawEnemy(GLint modelLoc, GLUquadricObj*& qobj, std::vector<Enemy>& g_enemies);
vec3 AStar(float playerx, float playerz, Enemy& enemy, int** maptile, int row, int column);
void EnemyDamageFunc(int value);
void EnemyDyingFunc(int value);
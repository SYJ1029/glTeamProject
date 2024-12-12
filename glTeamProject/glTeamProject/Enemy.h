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
};

//extern GLuint enemyVAO, enemyVBO, enemyEBO;


void InitEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies);
void MoveEnemy(float playerx, float playerz, std::vector<Enemy>& g_enemies);
void drawEnemy(GLint modelLoc, GLUquadricObj*& qobj, std::vector<Enemy>& g_enemies);
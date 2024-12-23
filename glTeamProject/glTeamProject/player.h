#pragma once

#include "sphere.h"
#include "shader.h"
#include "Hexahedron.h"
#include "pyramid.h"
#include "Enemy.h"

#include <gl/glew.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

using namespace glm;

extern GLuint playerVAO, playerVBO, playerEBO;

typedef struct Player {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	GLfloat angleXZ;
	GLfloat angleY;
	bool gun;
	vec3 gunMuzzleWorldPositionVec3;
};

void InitPlayer(GLUquadricObj* &qobj, Player &player);

void drawPlayer(GLint modelLoc, GLUquadricObj*& qobj, Player& player);

void applyGravity(Player &player);

void jump(Player& player);

void updatePlayer(Player &player, bool build);

void playerCollisionWithEnemy(Player& player, std::vector<Enemy>& g_enemies);

float GetNextXZ(Player& player, int option);
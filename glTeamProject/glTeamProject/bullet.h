#pragma once

#include "player.h"
#include "sphere.h"
#include "shader.h"
#include "Enemy.h"
#include <gl/glew.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace glm;

typedef struct Bullet {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	int damage;
};

void shootBullet(Player& player, std::vector<Bullet>& g_bullets, glm::vec3 cameraDirection);
void drawBullets(GLint modelLoc, Player& player, std::vector<Bullet>& g_bullets);
void updateBullets(std::vector<Bullet>& g_bullets);
void checkCollisionWithEnemies(std::vector<Bullet>& g_bullets, std::vector<Enemy>& g_enemies);

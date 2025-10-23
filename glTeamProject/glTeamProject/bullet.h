#pragma once

#include "pch.h"

#include "player.h"
#include "sphere.h"
#include "shader.h"
#include "Enemy.h"
#include "particle.h"

#include <vector>

using namespace glm;

typedef struct Bullet {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	int damage;
	bool damaged;
};

void shootBullet(Player& player, std::vector<Bullet>& g_bullets, glm::vec3 cameraDirection);
void drawBullets(GLint modelLoc, Player& player, std::vector<Bullet>& g_bullets);
void updateBullets(std::vector<Bullet>& g_bullets);
void checkCollisionWithEnemies(std::vector<Bullet>& g_bullets, std::vector<Enemy>& g_enemies);

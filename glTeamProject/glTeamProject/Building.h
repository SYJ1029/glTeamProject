#pragma once

#include "sphere.h"
#include "shader.h"
#include "readObj.h"
#include <gl/glew.h>
#include <gl/glew.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <stdlib.h>
#include "bullet.h"

using namespace std;
using namespace glm;

typedef struct Building {
	GLfloat x, y, z;
	Vertex scale;
};

extern GLuint buildVAO, buildVBO, buildEBO;

void InitBuliding(const char* objFilename, int** maptile, int& tilerow, int& tilecolumn, 
	int numBuild, std::vector<Building>& g_buildings);
void ConcatenateTile(int index, std::vector<Building>& g_buildings, float dx, float dz, int** maptile, int row, int column);
void drawBuliding(GLint modelLoc, std::vector<Building>& g_buildings, float dx, float dz, int** maptile, int row, int column);
bool BuildingCollisionPlayer(std::vector<Building>& g_buildings, Player& player);
bool BuildingCollisionBullet(std::vector<Building>& g_buildings, std::vector<Bullet>& g_bullets);
bool BuildingCollisionEnemy(std::vector<Building>& g_buildings, std::vector<Enemy>& g_enemies);
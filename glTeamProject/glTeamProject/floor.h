#pragma once

#include "pch.h"
#include "player.h"


using namespace glm;

extern GLuint floorVAO, floorVBO, floorEBO;

void InitFloor();

void drawFloor(GLint modelLoc, Player &player);

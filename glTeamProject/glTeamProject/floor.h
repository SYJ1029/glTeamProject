#pragma once

#include <gl/glew.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>

using namespace glm;

extern GLuint floorVAO, floorVBO, floorEBO;

void InitFloor();

void drawFloor(GLint modelLoc);

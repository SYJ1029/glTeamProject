#pragma once

#include <GL/glew.h>

#include <vector>

#define M_PI 3.141592

extern GLuint sphereVAO, sphereVBO;
extern int indexCount;

void initSphereBuffer(float radius, int stackCount, int sectorCount);

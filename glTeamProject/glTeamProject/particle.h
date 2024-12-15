#pragma once

#include "sphere.h"
#include "shader.h"
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <vector>
#include <cmath>

#define M_PI 3.141592

using namespace glm;

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    int life;
};

extern int maxParticles;

void addParticles(vec3 enemyPos);
void updateParticles();
void drawParticles(GLint modelLoc);

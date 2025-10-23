#pragma once

#include "pch.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace glm;



bool CollisionCheckCircle(glm::vec3 pos, glm::vec3 circlePos, float radius);
bool CollisionCheckRect(vec3 pos, vec3 rectPosLow, vec3 rectPosHigh);
bool ViewVolumeRect(vec3 pos, vec3 rectpos, vec3 rectPosLow, vec3 rectPosHigh);

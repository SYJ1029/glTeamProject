#pragma once

#include "Collision.h"

bool CollisionCheckCircle(glm::vec3 pos, glm::vec3 circlePos, float radius) {

	// 점 사이의 거리가 반지름보다 크다면 충돌
	if (distance(pos, circlePos) <= radius) return true;

	return false;
}


bool CollisionCheckRect(vec3 pos, vec3 rectPosLow, vec3 rectPosHigh) {

	// 지정한 점이 사각형 내부라면 충돌
	if (pos.x >= rectPosLow.x && pos.x <= rectPosHigh.x &&
		pos.z >= rectPosLow.z && pos.z <= rectPosHigh.z)
		return true;


	return false;
}
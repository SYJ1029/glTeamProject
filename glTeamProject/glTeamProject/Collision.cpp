#pragma once

#include "Collision.h"

bool CollisionCheckCircle(glm::vec3 pos, glm::vec3 circlePos, float radius) {

	// �� ������ �Ÿ��� ���������� ũ�ٸ� �浹
	if (distance(pos, circlePos) <= radius) return true;

	return false;
}


bool CollisionCheckRect(vec3 pos, vec3 rectPosLow, vec3 rectPosHigh) {

	// ������ ���� �簢�� ���ζ�� �浹
	if (pos.x >= rectPosLow.x && pos.x <= rectPosHigh.x &&
		pos.z >= rectPosLow.z && pos.z <= rectPosHigh.z)
		return true;


	return false;
}
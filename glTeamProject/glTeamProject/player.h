#pragma once

#include "pch.h"

#include "Object.h"

#include "sphere.h"
#include "shader.h"
#include "Hexahedron.h"
#include "pyramid.h"
#include "Enemy.h"



using namespace glm;

extern GLuint playerVAO, playerVBO, playerEBO;


//class Player {
//private:
//	GLfloat x, y, z;
//	GLfloat dx, dy, dz;
//	GLfloat angleXZ;
//	GLfloat angleY;
//	bool gun;
//	vec3 gunMuzzleWorldPositionVec3;
//
//public:
//	Player() {}
//	Player(GLUquadricObj*& qobj, Player& player);
//
//
//	virtual void Init(GLUquadricObj*& qobj, Player& player);
//	virtual void draw(GLint modelLoc, GLUquadricObj*& qobj, Player& player);
//
//	inline void applyGravity() {
//		if (dy > -0.5f) {
//			dy -= 0.02f;
//		}
//	}
//	void jump();
//
//	void update(bool build);
//
//	void playerCollisionWithEnemy(std::vector<Enemy>& g_enemies);
//	float GetNextXZ(int option);
//
//
//};

typedef struct Player {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	GLfloat angleXZ;
	GLfloat angleY;
	bool gun;
	vec3 gunMuzzleWorldPositionVec3;
};

void InitPlayer(GLUquadricObj* &qobj, Player &player);

void drawPlayer(GLint modelLoc, GLUquadricObj*& qobj, Player& player);

void applyGravity(Player &player);

void jump(Player& player);

void updatePlayer(Player &player, bool build);

void playerCollisionWithEnemy(Player& player, std::vector<Enemy>& g_enemies);

float GetNextXZ(Player& player, int option);
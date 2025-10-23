#pragma once


#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>


#pragma comment(lib, "glew32.lib")


//enum Direction {
//	UP,
//	DOWN,
//	LEFT,
//	RIGHT,
//	FORWARD,
//	BACKWARD
//};
//
//enum Axis {
//	X_AXIS,
//	Y_AXIS,
//	Z_AXIS
//};
//
//enum ObjectType {
//	PLAYER,
//	ENEMY,
//	BUILDING,
//	BULLET,
//	FLOOR,
//	PARTICLE
//};
//
//enum PlayerState {
//	STATE_IDLE,
//	STATE_WALKING,
//	STATE_JUMPING,
//	STATE_SHOOTING
//};
//
//struct GLPoint {
//	GLfloat x;
//	GLfloat y;
//	GLfloat z;
//
//
//	GLPoint(GLfloat xPos, GLfloat yPos, GLfloat zPos) : x(xPos), y(yPos), z(zPos) {}
//
//	inline GLPoint operator+(const GLPoint& other) const {
//		return GLPoint(x + other.x, y + other.y, z + other.z);
//	}
//	inline GLPoint operator-(const GLPoint& other) const {
//		return GLPoint(x - other.x, y - other.y, z - other.z);
//	}
//
//	inline GLPoint operator*(const GLfloat& scalar) const {
//		return GLPoint(x * scalar, y * scalar, z * scalar);
//	}
//
//	inline GLPoint operator/(const GLfloat& scalar) const {
//		if (scalar == 0) {
//			throw std::invalid_argument("Division by zero");
//		}
//
//		return GLPoint(x / scalar, y / scalar, z / scalar);
//	}
//};
//
//
//inline void applyGravity(GLfloat& dy, const GLfloat& Gravity, PlayerState& state) {
//	if (state == PlayerState::STATE_JUMPING) {
//		dy -= 0.02f;
//	}
//}

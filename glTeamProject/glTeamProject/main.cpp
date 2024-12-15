#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include "readObj.h"
#include "rwTile.h"
#include "shader.h"
#include "sphere.h"
#include "Hexahedron.h"
#include "player.h"
#include "bullet.h"
#include "floor.h"
#include "Enemy.h"
#include "Building.h"
#include "pyramid.h"
#include "particle.h"

using namespace glm;
using namespace std;

//--- ����� ���� �Լ�
GLvoid drawScene();
GLvoid Reshape(int w, int h);

//--- ���̴� ���� ����
GLint width, height;

GLuint buildVAO, buildVBO, buildEBO;
mat4 view;
mat4 projection;
//--- ���� ���� ����
GLUquadricObj* qobj = nullptr;
Player player;

std::vector<Enemy> g_enemies;
int genEnemyInterval = 5000;

std::vector<Building>g_buildings;
Model buildingModel;
int numBuild = 30;
int** maptile;
int tilerow = 50, tilecolumn = 50;
bool onNVD = false;

std::vector<Bullet>g_bullets;

float prevMouseX, prevMouseY;
float deltaX = 0.0f, deltaY = 0.0f;

vec3 cameraPos = vec3(0.0f, 8.0f, 0.0f);		//--- ī�޶� ��ġ
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- ī�޶� �ٶ󺸴� ����
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- ī�޶� ���� ����
vec3 directionVector;
float rotationSpeed = 1.0f;						// ī�޶� ȸ�� �ӵ� (deg/sec)

vec3 lightPos = vec3(0.0f, 200.0f, 0.0f);
float lightAngle = 0.0f;
float lightRadius = 200.0f;
float skyColor = 1.0f;
float ambientLight = 0.5f;

void setupCamera() {
	float radius = 1.0f;

	cameraPos.x = player.x + radius * cos(glm::radians(player.angleXZ)) * cos(glm::radians(player.angleY));
	cameraPos.y = player.y + 2.0f + radius * sin(glm::radians(-player.angleY)); // Y�� ��ġ�� player.y + 2 ����
	cameraPos.z = player.z + radius * sin(glm::radians(player.angleXZ)) * cos(glm::radians(player.angleY));

	cameraDirection.x = player.x + 2 * (radius * cos(glm::radians(player.angleXZ)));
	cameraDirection.y = cameraPos.y - 2 * (radius * sin(glm::radians(player.angleY)));
	cameraDirection.z = player.z + 2 * (radius * sin(glm::radians(player.angleXZ)));

	directionVector.x = cameraDirection.x - cameraPos.x;
	directionVector.y = cameraDirection.y - cameraPos.y;
	directionVector.z = cameraDirection.z - cameraPos.z;

	directionVector = glm::normalize(directionVector);

	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 175.0f);
}

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	// ���� ��ġ ������Ʈ
	lightAngle += 0.1f; // ȸ�� �ӵ� (deg/frame)
	if (lightAngle >= 360.0f) lightAngle -= 360.0f;
	ambientLight = 0.5f - 0.3f * (sin(glm::radians(lightAngle/2)));
	// ������ ��ġ�� ���
	lightPos.y = lightRadius * cos(glm::radians(lightAngle));
	lightPos.z = lightRadius * sin(glm::radians(lightAngle));

	skyColor = 1.0 - (1.0f * (sin(glm::radians(lightAngle / 2))));

	updatePlayer(player, BuildingCollisionPlayer(g_buildings, player));
	updateBullets(g_bullets);
	MoveEnemy(player.x, player.z, g_enemies, maptile, tilerow, tilecolumn);
	checkCollisionWithEnemies(g_bullets, g_enemies);
	playerCollisionWithEnemy(player, g_enemies);
	BuildingCollisionBullet(g_buildings, g_bullets);
	updateParticles();
	setupCamera();
	glutPostRedisplay();
	glutTimerFunc(30, timerFunc, 0);
}

void genEnemyFunc(int value) {
	InitEnemy(player.x, player.z, g_enemies); // ���� ����� �Լ��� �и�

	glutTimerFunc(genEnemyInterval, genEnemyFunc, 0); // ������ Ÿ�̸� �ݹ����� �и��ؼ� ���
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		if (player.dx < 0.3f) player.dx += 0.3f;
		break;
	case 'a':
		if(player.dz > -0.3f) player.dz -= 0.3f;
		break;
	case 's':
		if (player.dx > -0.3f) player.dx -= 0.3f;
		break;
	case 'd':
		if (player.dz < 0.3f) player.dz += 0.3f;
		break;
	case ' ':
		if (player.dy == 0.0f) {
			jump(player);
		}
		break;
	case '\t':
		player.gun = !player.gun;
		break;
	case '1':
		onNVD = !(onNVD);
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		player.dx = 0.0f;
		break;
	case 'a':
		player.dz = 0.0f;
		break;
	case 's':
		player.dx = 0.0f;
		break;
	case 'd':
		player.dz = 0.0f;
		break;

	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			shootBullet(player, g_bullets, directionVector);
		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	glutPostRedisplay();
}

void PassiveMotion(int x, int y) {
	int centerX = WINDOW_X / 2;
	int centerY = WINDOW_Y / 2;

	if (x != centerX || y != centerY) {
		deltaX = (float)(x - centerX);
		deltaY = (float)(y - centerY);

		float angleIncrement = 0.1f;
		player.angleXZ += deltaX * angleIncrement;
		player.angleY += deltaY * angleIncrement;
		if (player.angleY >= 15.0f) {
			player.angleY = 15.0f;
		}
		else if (player.angleY <= -15.0f) {
			player.angleY = -15.0f;
		}
		if (player.angleXZ >= 360.0f) player.angleXZ -= 360.0f;
		if (player.angleXZ < 0.0f) player.angleXZ += 360.0f;

		// ���콺 �����͸� ȭ�� �߾����� ����
		glutWarpPointer(centerX, centerY);
	}
}

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(time(NULL));
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("Example1");

	//--- GLEW �ʱ�ȭ�ϱ�
	glewExperimental = GL_TRUE;
	glewInit();

	//--- ���̴� �о�ͼ� ���̴� ���α׷� �����
	make_shaderProgram();
	InitPlayer(qobj, player);
	InitFloor();
	initSphereBuffer(1.0f, 20, 20);
	maptile = InitBuliding("obj.obj", maptile, tilerow, tilecolumn, numBuild,  g_buildings);
	Initbuffer();
	InitPyramidBuffer();
	setupCamera();
	glEnable(GL_DEPTH_TEST);

	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	glutTimerFunc(30, timerFunc, 0);
	glutTimerFunc(genEnemyInterval / 2, genEnemyFunc, 0);
	glutMainLoop();
}




//--- ��� �ݹ� �Լ�
GLvoid drawScene() {
	//--- ���� ���� �� ���� Ŭ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, skyColor, skyColor, 1.0f);

	//--- ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(shaderProgramID);

	//--- ����
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);		// ���� ��ġ ����
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 1.0f, 1.0f, 1.0f);						// ���� ����
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);	// ī�޶� ��ġ
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambientLight"), ambientLight); // �ֺ���
	glUniform1i(glGetUniformLocation(shaderProgramID, "objtype"), 0);

	//--- �Ȱ�
	glUniform1f(glGetUniformLocation(shaderProgramID, "fogStart"), 40.0f);
	glUniform1f(glGetUniformLocation(shaderProgramID, "fogEnd"), 50.0f);
	glUniform3f(glGetUniformLocation(shaderProgramID, "fogColor"), 0.0f, skyColor, skyColor); // ī�޶� ��ġ

	// �׸���
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glViewport(0, 0, WINDOW_X, WINDOW_Y); // ��ü ȭ��
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc, player);
	drawPlayer(modelLoc, qobj, player);
	drawEnemy(modelLoc, qobj, g_enemies);
	drawBuliding(modelLoc, g_buildings, player.x, player.z, maptile, tilerow, tilecolumn, onNVD);
	drawBullets(modelLoc, player, g_bullets);
	drawParticles(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);

	// �̴ϸ�
	glDisable(GL_DEPTH_TEST);
	glViewport(WINDOW_X * 3 / 4, WINDOW_Y * 3 / 4, WINDOW_X / 4, WINDOW_Y / 4); // ������ ��
	vec3 bodyModelPosV2 = vec3(player.x, 0.0f, player.z); // bodyModel�� �뷫���� ��ġ
	vec3 cameraPosV2 = vec3(player.x, 130.0f, player.z);
	mat4 bodyViewV2 = lookAt(cameraPosV2, bodyModelPosV2, vec3(cos(radians(player.angleXZ)), 0.0f, sin(radians(player.angleXZ)))); // bodyModel�� �ٶ󺸴� �� ���
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(bodyViewV2));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc, player);
	drawPlayer(modelLoc, qobj, player);
	drawEnemy(modelLoc, qobj, g_enemies);
	drawBuliding(modelLoc, g_buildings, player.x, player.z, maptile, tilerow, tilecolumn, onNVD);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	//--- ���� ����
	glutSwapBuffers();
}

//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void DeleteEnemy(int index) {
	g_enemies.erase(g_enemies.begin() + index);
}



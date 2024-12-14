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

using namespace glm;
using namespace std;

//--- 사용자 정의 함수
GLvoid drawScene();
GLvoid Reshape(int w, int h);

//--- 셰이더 변수 선언
GLint width, height;

GLuint buildVAO, buildVBO, buildEBO;
mat4 view;
mat4 projection;
//--- 전역 변수 선언
GLUquadricObj* qobj = nullptr;
Player player;

std::vector<Enemy>g_enemies;
int genEnemyInterval = 5000;


std::vector<Building>g_buildings;
Model buildingModel;
int numBuild = 30;
int** maptile;
int tilerow = 50, tilecolumn = 50;

std::vector<Bullet>g_bullets;

float prevMouseX, prevMouseY;
float deltaX = 0.0f, deltaY = 0.0f;

vec3 cameraPos = vec3(0.0f, 8.0f, 0.0f);		//--- 카메라 위치
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- 카메라 바라보는 방향
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- 카메라 위쪽 방향
float rotationSpeed = 1.0f;						// 카메라 회전 속도 (deg/sec)

vec3 lightPos = vec3(0.0f, 200.0f, 0.0f);
float lightAngle = 0.0f;
float lightRadius = 200.0f;
float skyColor = 1.0f;
float ambientLight = 0.5f;

void setupCamera() {
	float radius = 1.0f;

	cameraPos.x = player.x + radius * cos(glm::radians(player.angleXZ));
	cameraPos.y = player.y + 2.0f;
	cameraPos.z = player.z + radius * sin(glm::radians(player.angleXZ));

	cameraDirection.x = player.x + 2 * (radius * cos(glm::radians(player.angleXZ)));
	cameraDirection.y = cameraPos.y;
	cameraDirection.z = player.z + 2 * (radius * sin(glm::radians(player.angleXZ)));

	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 175.0f);
}

// 타이머 함수
void timerFunc(int value) {
	// 조명 위치 업데이트
	lightAngle += 0.1f; // 회전 속도 (deg/frame)
	if (lightAngle >= 360.0f) lightAngle -= 360.0f;
	ambientLight = 0.5f - 0.3f * (sin(glm::radians(lightAngle/2)));
	// 조명의 위치를 계산
	lightPos.y = lightRadius * cos(glm::radians(lightAngle));
	lightPos.z = lightRadius * sin(glm::radians(lightAngle));

	skyColor = 1.0 - (1.0f * (sin(glm::radians(lightAngle / 2))));

	updatePlayer(player, BuildingCollisionPlayer(g_buildings, player));
	updateBullets(g_bullets);
	MoveEnemy(player.x, player.z, g_enemies);
	checkCollisionWithEnemies(g_bullets, g_enemies);
	playerCollisionWithEnemy(player, g_enemies);
	BuildingCollisionBullet(g_buildings, g_bullets);
	setupCamera();
	glutPostRedisplay();
	glutTimerFunc(30, timerFunc, 0);
}

void genEnemyFunc(int value) {
	InitEnemy(player.x, player.z, g_enemies); // 적을 만드는 함수는 분리

	glutTimerFunc(genEnemyInterval, genEnemyFunc, 0); // 별개의 타이머 콜백으로 분리해서 사용
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
			shootBullet(player, g_bullets);
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

		if (player.angleXZ >= 360.0f) player.angleXZ -= 360.0f;
		if (player.angleXZ < 0.0f) player.angleXZ += 360.0f;

		// 마우스 포인터를 화면 중앙으로 워핑
		glutWarpPointer(centerX, centerY);
	}
}

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WINDOW_X, WINDOW_Y);
	glutCreateWindow("Example1");

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	glewInit();

	//--- 세이더 읽어와서 세이더 프로그램 만들기
	make_shaderProgram();
	InitPlayer(qobj, player);
	InitFloor();
	initSphereBuffer(1.0f, 20, 20);
	InitBuliding("obj.obj", maptile, tilerow, tilecolumn, numBuild,  g_buildings);
	Initbuffer();
	InitPyramidBuffer();
	setupCamera();
	glEnable(GL_DEPTH_TEST);

	//--- 세이더 프로그램 만들기
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




//--- 출력 콜백 함수
GLvoid drawScene() {
	//--- 배경색 설정 및 버퍼 클리어
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, skyColor, skyColor, 1.0f);

	//--- 셰이더 프로그램 활성화
	glUseProgram(shaderProgramID);

	//--- 조명
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);		// 조명 위치 고정
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 1.0f, 1.0f, 1.0f);						// 조명 색상
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);	// 카메라 위치
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambientLight"), ambientLight);						// 주변광

	//--- 안개
	glUniform1f(glGetUniformLocation(shaderProgramID, "fogStart"), 40.0f);
	glUniform1f(glGetUniformLocation(shaderProgramID, "fogEnd"), 50.0f);
	glUniform3f(glGetUniformLocation(shaderProgramID, "fogColor"), 0.0f, skyColor, skyColor); // 카메라 위치

	// 그리기
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glViewport(0, 0, WINDOW_X, WINDOW_Y); // 전체 화면
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc, player);
	drawPlayer(modelLoc, qobj, player);
	drawEnemy(modelLoc, qobj, g_enemies);
	drawBuliding(modelLoc, g_buildings, player.x, player.z, maptile, tilerow, tilecolumn);
	drawBullets(modelLoc, player, g_bullets);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);

	// 미니맵
	glDisable(GL_DEPTH_TEST);
	glViewport(WINDOW_X * 3 / 4, WINDOW_Y * 3 / 4, WINDOW_X / 4, WINDOW_Y / 4); // 오른쪽 위
	vec3 bodyModelPosV2 = vec3(player.x, 0.0f, player.z); // bodyModel의 대략적인 위치
	vec3 cameraPosV2 = vec3(player.x, 10.0f, player.z);
	mat4 bodyViewV2 = lookAt(cameraPosV2, bodyModelPosV2, vec3(cos(radians(player.angleXZ)), 0.0f, sin(radians(player.angleXZ)))); // bodyModel을 바라보는 뷰 행렬
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(bodyViewV2));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc, player);
	drawPlayer(modelLoc, qobj, player);
	drawEnemy(modelLoc, qobj, g_enemies);
	drawBuliding(modelLoc, g_buildings, player.x, player.z, maptile, tilerow, tilecolumn);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	//--- 버퍼 스왑
	glutSwapBuffers();
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void DeleteEnemy(int index) {
	g_enemies.erase(g_enemies.begin() + index);
}




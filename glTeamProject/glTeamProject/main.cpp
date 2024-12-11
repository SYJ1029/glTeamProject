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

using namespace glm;
using namespace std;

//--- ����� ���� �Լ�
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitBuliding(const char* objFilename);
//--- ���̴� ���� ����
GLint width, height;

GLuint buildVAO, buildVBO, buildEBO;
mat4 view;
mat4 projection;
//--- ���� ���� ����
GLUquadricObj* qobj = nullptr;
Player player;

std::vector<Enemy>g_enemies;
int genEnemyInterval = 5000;

typedef struct Building {
	GLfloat x, y, z;
	Vertex scale;
};
std::vector<Building>g_buildings;
Model buildingModel;
int numBuild = 30;
int** maptile;
int tilerow = 50, tilecolumn = 50;

std::vector<Bullet>g_bullets;

float prevMouseX, prevMouseY;
float deltaX = 0.0f, deltaY = 0.0f;

vec3 cameraPos = vec3(0.0f, 8.0f, 0.0f);		//--- ī�޶� ��ġ
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- ī�޶� �ٶ󺸴� ����
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- ī�޶� ���� ����
float rotationSpeed = 1.0f;						// ī�޶� ȸ�� �ӵ� (deg/sec)

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

// Ÿ�̸� �Լ�
void timerFunc(int value) {
	updatePlayer(player);
	// ���� ��ġ ������Ʈ
	lightAngle += 0.1f; // ȸ�� �ӵ� (deg/frame)
	if (lightAngle >= 360.0f) lightAngle -= 360.0f;
	ambientLight = 0.5f - 0.3f * (sin(glm::radians(lightAngle/2)));
	// ������ ��ġ�� ���
	lightPos.y = lightRadius * cos(glm::radians(lightAngle));
	lightPos.z = lightRadius * sin(glm::radians(lightAngle));

	skyColor = 1.0 - (1.0f * (sin(glm::radians(lightAngle / 2))));

	updateBullets(g_bullets);
	checkCollisionWithEnemies(g_bullets, g_enemies);
	MoveEnemy(player.x, player.z, g_enemies);
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
	initSphereBuffer(0.8f, 20, 20);
	InitBuliding("obj.obj");
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

void ConcatenateTile(int index) {
	float clipx = abs(player.x) - 50.0f;
	float clipz = abs(player.z) - 50.0f;
	
	if (clipx > 0 && abs(g_buildings[index].x) < 100.0f - clipx) { // floor�� Ŭ������ ������ Tile�� ������ ���
		if (player.x < 0)  {
			// player.x < 0 �� �������� �߷����� �ǹ�
			
			g_buildings[index].x -= 100.0f; // ������ �̵��Ѵ�.

		}
		else { // player.x = 0�� ���� ����� �ʿ䰡 ����.
			g_buildings[index].x += 100.0f; // �ݴ� ������ �̵��Ѵ�.
		}
	}

	if (clipz > 0 && abs(g_buildings[index].z) < 100.0f - clipz) { // ��� 2
		if (player.z < 0) {
			// player.z< 0 �� �������� �߷����� �ǹ�

			g_buildings[index].z -= 100.0f;
		}
		else{
			g_buildings[index].z += 100.0f;
	}
	}
}

void drawBuliding(GLint modelLoc) {
	glBindVertexArray(buildVAO);
	for (int i = 0; i < g_buildings.size(); i++) {
		ConcatenateTile(i);
		if (g_buildings[i].x >= -50.0f + player.x && g_buildings[i].x <= 50.0f + player.x &&
			g_buildings[i].z >= -50.0f + player.z && g_buildings[i].z <= 50.0f + player.z) {
			mat4 buildingModelMat = mat4(1.0f);
			buildingModelMat = translate(buildingModelMat, vec3(g_buildings[i].x, g_buildings[i].y, g_buildings[i].z));
			buildingModelMat = scale(buildingModelMat, vec3(g_buildings[i].scale.x, g_buildings[i].scale.y, g_buildings[i].scale.z));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(buildingModelMat));
			glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 1.0f);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	glBindVertexArray(0);
}

//--- ��� �ݹ� �Լ�
GLvoid drawScene() {
	//--- ���� ���� �� ���� Ŭ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, skyColor, skyColor, 1.0f);

	//--- ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(shaderProgramID);

	//--- ����
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z); // ���� ��ġ ����
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 1.0f, 1.0f, 1.0f); // ���� ����
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z); // ī�޶� ��ġ
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambientLight"), ambientLight); // �ֺ���

	// �׸���
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glViewport(0, 0, WINDOW_X, WINDOW_Y); // ��ü ȭ��
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc, player);
	drawPlayer(modelLoc, qobj, player);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawEnemy(modelLoc, qobj, g_enemies);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawBuliding(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawBullets(modelLoc, player, g_bullets);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);

	// �̴ϸ�
	glViewport(WINDOW_X * 3 / 4, WINDOW_Y * 3 / 4, WINDOW_X / 4, WINDOW_Y / 4); // ������ ��
	vec3 bodyModelPosV2 = vec3(player.x, 0.0f, player.z); // bodyModel�� �뷫���� ��ġ
	vec3 cameraPosV2 = vec3(player.x, 35.0f, player.z);
	mat4 bodyViewV2 = lookAt(cameraPosV2, bodyModelPosV2, vec3(cos(radians(player.angleXZ)), 0.0f, sin(radians(player.angleXZ)))); // bodyModel�� �ٶ󺸴� �� ���
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(bodyViewV2));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc, player);
	drawPlayer(modelLoc, qobj, player);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawEnemy(modelLoc, qobj, g_enemies);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawBuliding(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);


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

void SetTile() {
	int i = -1;
	int j = -1;

	while (true) {
		i = (int)((float)rand() / RAND_MAX * (tilerow - 1));
		j = (int)((float)rand() / RAND_MAX * (tilecolumn - 1));

		if (maptile[i][j] <= 0) {
			maptile[i][j] = 1;

			break;
		}
	} 

}

void InitBuliding(const char* objFilename) {
	maptile = InitTileArr(maptile, tilerow, tilecolumn);

	for (int i = 0; i < numBuild; i++) {
		SetTile();
	}


	Building building;

	read_obj_file(objFilename, &buildingModel);

	for (int i = 0; i < tilerow; i++) {
		for (int j = 0; j < tilecolumn; j++) {
			if (maptile[i][j] > 0) {
				building = { (float)i * 5 - 100.0f, 0.0f, (float)j * 5 - 100.0f, 5.0f, 10.0f, 5.0f };
				g_buildings.push_back(building);
			}
		}
	}




	glGenVertexArrays(1, &buildVAO);
	glGenBuffers(1, &buildVBO);
	glGenBuffers(1, &buildEBO);

	glBindVertexArray(buildVAO);

	//VBO�� ������ ���
	glBindBuffer(GL_ARRAY_BUFFER, buildVBO);
	glBufferData(GL_ARRAY_BUFFER, 40 * sizeof(buildingModel.vertices), buildingModel.vertices, GL_STATIC_DRAW);
	cout << sizeof(buildingModel.vertices) << endl;

	//EBO�� ������ ���
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buildEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 40 * sizeof(buildingModel.faces), buildingModel.faces, GL_STATIC_DRAW);
	cout << sizeof(buildingModel.faces) << endl;


	// ��ġ �Ӽ� ���� (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO ����ε�
	glBindVertexArray(0);
}

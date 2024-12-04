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
#include "shader.h"
#include "sphere.h"
#include "Hexahedron.h"

using namespace glm;
using namespace std;

//--- �Ʒ� 5�� �Լ��� ����� ���� �Լ�
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitFloor();
void InitPlayer();
void InitBuliding(const char* objFilename);
void shootBullet();
void drawBullets(GLint modelLoc);
void updateBullets();
//--- ���̴� ���� ����
GLint width, height;
GLuint floorVAO, floorVBO, floorEBO;
GLuint playerVAO, playerVBO, playerEBO;
GLuint enemyVAO, enemyVBO, enemyEBO;
GLuint buildVAO, buildVBO, buildEBO;
mat4 view;
mat4 projection;
//--- ���� ���� ����
GLUquadricObj* qobj;

typedef struct Player {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	GLfloat angleXZ;
	GLfloat angleY;
};
Player player;

typedef struct Enemy {
	GLfloat x, y, z;
	// GLfloat angle;
};
std::vector<Enemy>g_enemies;

typedef struct Building {
	GLfloat x, y, z;
	Vertex scale;
};
std::vector<Building>g_buildings;
Model buildingModel;
int numBuild = 10;

typedef struct Bullet {
	GLfloat x, y, z;
	GLfloat dx, dy, dz;
	GLfloat speed;
};
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
	// �÷��̾� ī�޶� ������Ʈ
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)),  // X�� ����
		0.0f,                              // Y�� (����)
		sin(glm::radians(player.angleXZ))  // Z�� ����
	));
	player.x += player.dx * direction.x - player.dz * direction.z;
	player.z += player.dx * direction.z + player.dz * direction.x;
	//player.y += player.dy;

	// ���� ��ġ ������Ʈ
	lightAngle += 0.1f; // ȸ�� �ӵ� (deg/frame)
	if (lightAngle >= 360.0f) lightAngle -= 360.0f;
	ambientLight = 0.5f - 0.3f * (sin(glm::radians(lightAngle/2)));
	// ������ ��ġ�� ���
	lightPos.y = lightRadius * cos(glm::radians(lightAngle));
	lightPos.z = lightRadius * sin(glm::radians(lightAngle));

	skyColor = 1.0 - (1.0f * (sin(glm::radians(lightAngle / 2))));

	updateBullets();
	setupCamera();
	glutPostRedisplay();
	glutTimerFunc(30, timerFunc, 0);
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
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void keyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		player.dx -= 0.3f;
		break;
	case 'a':
		player.dz += 0.3f;
		break;
	case 's':
		player.dx += 0.3f;
		break;
	case 'd':
		player.dz -= 0.3f;
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			shootBullet();
		}
		else if (state == GLUT_UP) {

		}
	}
	glutPostRedisplay();
}

void Motion(int x, int y) {
	glutPostRedisplay();
}

void PassiveMotion(int x, int y) {
	float mouseX = (2.0f * x / WINDOW_X) - 1.0f;
	float mouseY = 1.0f - (2.0f * y / WINDOW_Y);

	static bool firstMouse = true;
	if (firstMouse) {
		prevMouseX = mouseX;
		prevMouseY = mouseY;
		firstMouse = false; // ���ĺ��ʹ� �ʱ�ȭ ���ʿ�
		return; // �ʱ�ȭ �� ù ����� �ǳʶ�
	}

	deltaX = mouseX - prevMouseX;
	deltaY = mouseY - prevMouseY;

	float angleIncrement = 90.0f;
	player.angleXZ += deltaX * angleIncrement;

	if (player.angleXZ >= 360.0f) player.angleXZ -= 360.0f;
	if (player.angleXZ < 0.0f) player.angleXZ += 360.0f;

	prevMouseX = mouseX;
	prevMouseY = mouseY;

	printf("Mouse moved: deltaX = %.3f, player.angleXZ = %.3f\r", deltaX, player.angleXZ);

	glutPostRedisplay();
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
	InitPlayer();
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
	glutMainLoop();
}

void drawFloor(GLint modelLoc) {
	// �ٴ�
	glBindVertexArray(floorVAO);
	mat4 floorModel = mat4(1.0f); // �� ���
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(floorModel));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	
}

void drawPlayer(GLint modelLoc) {
	glBindVertexArray(sphereVAO);

	mat4 playerModelMat = mat4(1.0f); // �÷��̾� �� ���
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f,0.0f,0.0f);

	playerModelMat = glm::translate(playerModelMat, vec3(player.x, player.y + 2.0f, player.z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	playerModelMat = glm::rotate(playerModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	playerModelMat = glm::translate(playerModelMat, vec3(0.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	gluCylinder(qobj, 1.0, 0.3, 1.5, 20, 8);

	glBindVertexArray(0); // VAO ����ε�
}

void drawEnemy(GLint modelLoc) {
	mat4 baseModelMat = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.0f)); // ���� �׷����� ���� ��ü�� ���� �̵�

	mat4 enemyModelMat = mat4(1.0f); // �� �� ���
	enemyModelMat *= baseModelMat;
	enemyModelMat = glm::rotate(enemyModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(enemyModelMat));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.5f, 1.0f);

	gluCylinder(qobj, 0.75f, 0.75f, 1.5f, 20.0f, 8.0f);

	enemyModelMat = glm::translate(enemyModelMat,  vec3(0.0f, 0.0f, 0.0f));
	enemyModelMat *= baseModelMat; // ȸ���� ���� ����

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(enemyModelMat));

	gluSphere(qobj, 0.8, 50, 50);

	glBindVertexArray(enemyVAO);
	glBindVertexArray(0); // VAO ����ε�
}

void drawBuliding(GLint modelLoc) {
	mat4 buildingModelMat = mat4(1.0f);
	glBindVertexArray(buildVAO);
	for (int i = 0; i < g_buildings.size(); i++) {
		buildingModelMat = mat4(1.0f);
		buildingModelMat *= translate(buildingModelMat, vec3(g_buildings[i].x, g_buildings[i].y, g_buildings[i].z));
		buildingModelMat *= scale(buildingModelMat, vec3(g_buildings[i].scale.x, g_buildings[i].scale.y, g_buildings[i].scale.z));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(buildingModelMat));
		glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 1.0f);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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

	drawFloor(modelLoc);
	drawPlayer(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawEnemy(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawBuliding(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawBullets(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);

	// �̴ϸ�
	glViewport(WINDOW_X * 3 / 4, WINDOW_Y * 3 / 4, WINDOW_X / 4, WINDOW_Y / 4); // ������ ��
	vec3 bodyModelPosV2 = vec3(player.x, 0.0f, player.z); // bodyModel�� �뷫���� ��ġ
	vec3 cameraPosV2 = vec3(player.x, 10.0f, player.z);
	mat4 bodyViewV2 = lookAt(cameraPosV2, bodyModelPosV2, vec3(1.0f, 0.0f, 0.0f)); // bodyModel�� �ٶ󺸴� �� ���
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(bodyViewV2));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc);
	drawPlayer(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawEnemy(modelLoc);
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

void InitFloor() {
	// �ٴ� ���� ������
	GLfloat floorVertices[] = {
		// Positions          // Colors
		-50.0f, 0.0f, -50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0.0f,  50.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	};

	// �ٴ��� �ε��� ������
	GLuint floorIndices[] = {
		0, 1, 2, // ù ��° �ﰢ��
		0, 3, 2  // �� ��° �ﰢ��
	};

	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	// VBO ���� �� ������ ����
	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) ���� �� ������ ����
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices),floorIndices, GL_STATIC_DRAW);

	// ��ġ �Ӽ� (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// ���� �Ӽ� (attribute 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// ���� �Ӽ� (attribute 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// VAO ����ε�
	glBindVertexArray(0);
}

void InitPlayer() {
	// �÷��̾� �𵨿� Quadric ����
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	
	glBindVertexArray(0); // VAO ����ε�
	player.x = 0.0f;
	player.y = 0.0f;
	player.z = 0.0f;
	player.dx = 0.0f;
	player.dy = 0.0f;
	player.dz = 0.0f;
	player.angleXZ = 0.0f;
	player.angleY = 0.0f;
}

GLfloat GetRandomNumber(int seed) {
	// ������� ������ [-seed/2, seed/2]
	float result = (float)((float)rand() / RAND_MAX) * seed - ((float)seed / 2.0f);

	return result;
}

void InitBuliding(const char* objFilename) {
	Building building;

	read_obj_file(objFilename, &buildingModel);

	building = { 23.0f, 0.0f, -22.0f, 4.0f, 4.0f, 4.0f};
	g_buildings.push_back(building);
	building = { -10.0f, 0.0f, -12.0f, 8.0f, 4.0f, 4.0f};
	g_buildings.push_back(building);
	building = { 13.0f, 0.0f, 20.0f, 4.0f, 8.0f, 8.0f };
	g_buildings.push_back(building);
	building = { -9.0f, 0.0f, 8.0f, 4.0f, 4.0f, 8.0f };
	g_buildings.push_back(building);
	building = { 16.0f, 0.0f, -23.0f, 6.0f, 4.0f, 4.0f };
	g_buildings.push_back(building);
	building = { 13.0f, 0.0f, -16.0f, 4.0f, 4.0f, 4.0f };
	g_buildings.push_back(building);
	building = { -4.0f, 0.0f, 2.0f, 8.0f, 8.0f, 8.0f };
	g_buildings.push_back(building);
	building = { -5.0f, 0.0f, -10.0f, 4.0f, 4.0f, 4.0f };
	g_buildings.push_back(building);
	building = { -10.0f, 0.0f, 20.0f, 4.0f, 4.0f, 6.0f };
	g_buildings.push_back(building);
	building = { -22.0f, 0.0f, -14.0f, 6.0f, 8.0f, 2.0f };
	g_buildings.push_back(building);




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

void shootBullet() {
	Bullet newBullet;
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)), // X�� ����
		0.0f,                              // Y�� (����)
		sin(glm::radians(player.angleXZ))  // Z�� ����
	));

	newBullet = { player.x, player.y + 1.95f, player.z };

	newBullet.dx = direction.x;
	newBullet.dy = direction.y;
	newBullet.dz = direction.z;

	newBullet.speed = 0.8f;

	g_bullets.push_back(newBullet);
}

void updateBullets() {
	for (int i = 0; i < g_bullets.size(); i++) {
		g_bullets[i].x += g_bullets[i].dx * g_bullets[i].speed;
		g_bullets[i].y += g_bullets[i].dy * g_bullets[i].speed;
		g_bullets[i].z += g_bullets[i].dz * g_bullets[i].speed;

		if (g_bullets[i].x > 100.0f || g_bullets[i].x < -100.0f ||
			g_bullets[i].z > 100.0f || g_bullets[i].z < -100.0f) {
			g_bullets.erase(g_bullets.begin() + i);
			i--;
		}
	}
}

void drawBullets(GLint modelLoc) {
	float radius = 0.1f;
	glBindVertexArray(sphereVAO);

	mat4 bulletModel;
	for (const Bullet& bullet : g_bullets) {
		bulletModel = mat4(1.0f);
		bulletModel = translate(bulletModel, vec3(bullet.x + radius * glm::cos(radians(player.angleXZ + 90.0f)), bullet.y, bullet.z + radius * glm::sin(radians(player.angleXZ + 90.0f))));
		bulletModel = scale(bulletModel, vec3(0.01f, 0.01f, 0.01f)); // �Ѿ� ũ�� ����

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(bulletModel));
		glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 1.0f, 0.0f); // ����� �Ѿ�

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

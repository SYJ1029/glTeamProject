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

//--- 아래 5개 함수는 사용자 정의 함수
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitFloor();
void InitPlayer();
void InitBuliding(const char* objFilename);
void shootBullet();
void drawBullets(GLint modelLoc);
void updateBullets();
//--- 셰이더 변수 선언
GLint width, height;
GLuint floorVAO, floorVBO, floorEBO;
GLuint playerVAO, playerVBO, playerEBO;
GLuint enemyVAO, enemyVBO, enemyEBO;
GLuint buildVAO, buildVBO, buildEBO;
mat4 view;
mat4 projection;
//--- 전역 변수 선언
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
	// 플레이어 카메라 업데이트
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)),  // X축 성분
		0.0f,                              // Y축 (고정)
		sin(glm::radians(player.angleXZ))  // Z축 성분
	));
	player.x += player.dx * direction.x - player.dz * direction.z;
	player.z += player.dx * direction.z + player.dz * direction.x;
	//player.y += player.dy;

	// 조명 위치 업데이트
	lightAngle += 0.1f; // 회전 속도 (deg/frame)
	if (lightAngle >= 360.0f) lightAngle -= 360.0f;
	ambientLight = 0.5f - 0.3f * (sin(glm::radians(lightAngle/2)));
	// 조명의 위치를 계산
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
		firstMouse = false; // 이후부터는 초기화 불필요
		return; // 초기화 후 첫 계산을 건너뜀
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
	InitPlayer();
	InitFloor();
	initSphereBuffer(0.8f, 20, 20);
	InitBuliding("obj.obj");
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
	glutMainLoop();
}

void drawFloor(GLint modelLoc) {
	// 바닥
	glBindVertexArray(floorVAO);
	mat4 floorModel = mat4(1.0f); // 모델 행렬
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(floorModel));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	
}

void drawPlayer(GLint modelLoc) {
	glBindVertexArray(sphereVAO);

	mat4 playerModelMat = mat4(1.0f); // 플레이어 모델 행렬
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f,0.0f,0.0f);

	playerModelMat = glm::translate(playerModelMat, vec3(player.x, player.y + 2.0f, player.z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	playerModelMat = glm::rotate(playerModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	playerModelMat = glm::translate(playerModelMat, vec3(0.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	gluCylinder(qobj, 1.0, 0.3, 1.5, 20, 8);

	glBindVertexArray(0); // VAO 언바인딩
}

void drawEnemy(GLint modelLoc) {
	mat4 baseModelMat = glm::translate(mat4(1.0f), vec3(0.0f, 0.0f, 2.0f)); // 적이 그려지는 도형 전체에 대한 이동

	mat4 enemyModelMat = mat4(1.0f); // 적 모델 행렬
	enemyModelMat *= baseModelMat;
	enemyModelMat = glm::rotate(enemyModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(enemyModelMat));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.5f, 1.0f);

	gluCylinder(qobj, 0.75f, 0.75f, 1.5f, 20.0f, 8.0f);

	enemyModelMat = glm::translate(enemyModelMat,  vec3(0.0f, 0.0f, 0.0f));
	enemyModelMat *= baseModelMat; // 회전한 흔적 제거

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(enemyModelMat));

	gluSphere(qobj, 0.8, 50, 50);

	glBindVertexArray(enemyVAO);
	glBindVertexArray(0); // VAO 언바인딩
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

//--- 출력 콜백 함수
GLvoid drawScene() {
	//--- 배경색 설정 및 버퍼 클리어
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, skyColor, skyColor, 1.0f);

	//--- 셰이더 프로그램 활성화
	glUseProgram(shaderProgramID);

	//--- 조명
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z); // 조명 위치 고정
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), 1.0f, 1.0f, 1.0f); // 조명 색상
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), cameraPos.x, cameraPos.y, cameraPos.z); // 카메라 위치
	glUniform1f(glGetUniformLocation(shaderProgramID, "ambientLight"), ambientLight); // 주변광

	// 그리기
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glViewport(0, 0, WINDOW_X, WINDOW_Y); // 전체 화면
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

	// 미니맵
	glViewport(WINDOW_X * 3 / 4, WINDOW_Y * 3 / 4, WINDOW_X / 4, WINDOW_Y / 4); // 오른쪽 위
	vec3 bodyModelPosV2 = vec3(player.x, 0.0f, player.z); // bodyModel의 대략적인 위치
	vec3 cameraPosV2 = vec3(player.x, 10.0f, player.z);
	mat4 bodyViewV2 = lookAt(cameraPosV2, bodyModelPosV2, vec3(1.0f, 0.0f, 0.0f)); // bodyModel을 바라보는 뷰 행렬
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(bodyViewV2));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc);
	drawPlayer(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawEnemy(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);
	drawBuliding(modelLoc);
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);


	//--- 버퍼 스왑
	glutSwapBuffers();
}

//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void InitFloor() {
	// 바닥 정점 데이터
	GLfloat floorVertices[] = {
		// Positions          // Colors
		-50.0f, 0.0f, -50.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f, -50.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		 50.0f, 0.0f,  50.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
		-50.0f, 0.0f,  50.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	};

	// 바닥의 인덱스 데이터
	GLuint floorIndices[] = {
		0, 1, 2, // 첫 번째 삼각형
		0, 3, 2  // 두 번째 삼각형
	};

	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);

	// VBO 생성 및 데이터 전송
	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// EBO (Element Buffer Object) 생성 및 데이터 전송
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices),floorIndices, GL_STATIC_DRAW);

	// 위치 속성 (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// 색상 속성 (attribute 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// 법선 속성 (attribute 2)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	// VAO 언바인딩
	glBindVertexArray(0);
}

void InitPlayer() {
	// 플레이어 모델용 Quadric 생성
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);
	
	glBindVertexArray(0); // VAO 언바인딩
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
	// 결과값의 범위는 [-seed/2, seed/2]
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

	//VBO에 데이터 등록
	glBindBuffer(GL_ARRAY_BUFFER, buildVBO);
	glBufferData(GL_ARRAY_BUFFER, 40 * sizeof(buildingModel.vertices), buildingModel.vertices, GL_STATIC_DRAW);
	cout << sizeof(buildingModel.vertices) << endl;

	//EBO에 데이터 등록
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buildEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 40 * sizeof(buildingModel.faces), buildingModel.faces, GL_STATIC_DRAW);
	cout << sizeof(buildingModel.faces) << endl;


	// 위치 속성 지정 (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO 언바인딩
	glBindVertexArray(0);
}

void shootBullet() {
	Bullet newBullet;
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)), // X축 방향
		0.0f,                              // Y축 (수평)
		sin(glm::radians(player.angleXZ))  // Z축 방향
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
		bulletModel = scale(bulletModel, vec3(0.01f, 0.01f, 0.01f)); // 총알 크기 조정

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(bulletModel));
		glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 1.0f, 0.0f); // 노란색 총알

		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <time.h>
#include <math.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include "readObj.h"

#define MAX_LINE_LENGTH 100
#define M_PI 3.141592
#define WINDOW_X 800
#define WINDOW_Y 600

using namespace glm;
using namespace std;

//--- 아래 5개 함수는 사용자 정의 함수
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitFloor();
void InitPlayer();
void InitBuliding(const char* objFilename);
char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) {	// Return NULL on failure 
		perror("ERROR: 쉐이더 파일을 열 수 없습니다.");
		return NULL;
	}
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
}

//--- 셰이더 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램
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

float prevMouseX, prevMouseY;
float deltaX = 0.0f, deltaY = 0.0f;

vec3 cameraPos = vec3(0.0f, 8.0f, 0.0f);			//--- 카메라 위치
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- 카메라 바라보는 방향
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- 카메라 위쪽 방향
float rotationSpeed = 1.0f;						// 카메라 회전 속도 (deg/sec)

void setupCamera() {
	float radius = 1.0f;

	//cameraPos.x = player.x + radius * cos(glm::radians(player.angleXZ));
	//cameraPos.y = player.y + 2.0f;
	//cameraPos.z = player.z + radius * sin(glm::radians(player.angleXZ));

	//cameraDirection.x = player.x + 2 * (radius * cos(glm::radians(player.angleXZ)));
	//cameraDirection.y = cameraPos.y;
	//cameraDirection.z = player.z + 2 * (radius * sin(glm::radians(player.angleXZ)));

	cameraPos = vec3(0.0f, 125.0f, 0.0f);			//--- 카메라 위치
	cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- 카메라 바라보는 방향
	cameraUp = vec3(1.0f, 0.0f, 0.0f);			//--- 카메라 위쪽 방향

	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 175.0f);
}

// 타이머 함수
void timerFunc(int value) {
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)),  // X축 성분
		0.0f,                              // Y축 (고정)
		sin(glm::radians(player.angleXZ))  // Z축 성분
	));

	player.x += player.dx * direction.x - player.dz * direction.z;
	player.z += player.dx * direction.z + player.dz * direction.x;
	player.y += player.dy;

	setupCamera();
	glutPostRedisplay();
	glutTimerFunc(100, timerFunc, 0);
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
	glutTimerFunc(1000, timerFunc, 0);
	glutMainLoop();
}

//--- 버텍스 세이더 객체 만들기
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	if (!vertexSource) {
		std::cerr << "ERROR: vertex shader 파일을 불러오지 못했습니다." << std::endl;
	}
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//checkCompileErrors(vertexShader, "VERTEX");
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 프래그먼트 세이더 객체 만들기
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	if (!fragmentSource) {
		std::cerr << "ERROR: fragment shader 파일을 불러오지 못했습니다." << std::endl;
	}
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

//--- 세이더 프로그램 만들고 세이더 객체 링크하기
void make_shaderProgram()
{
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기

	//-- shader Program
	shaderProgramID = glCreateProgram();

	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);


	//--- 세이더 삭제하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);

	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}





void drawFloor(GLint modelLoc) {
	// 바닥
	glBindVertexArray(floorVAO);
	mat4 floorModel = mat4(1.0f); // 모델 행렬 (변환 없음)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(floorModel));

	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.8f, 0.5f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);	
}

void drawPlayer(GLint modelLoc) {
	mat4 playerModelMat = mat4(1.0f); // 플레이어 모델 행렬
	playerModelMat = glm::rotate(playerModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));

	playerModelMat = glm::translate(playerModelMat, vec3(player.x, player.y, player.z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.8f, 0.5f);
	gluCylinder(qobj, 1.0, 0.3, 1.5, 20, 8);

	playerModelMat = glm::translate(playerModelMat, vec3(player.x, player.y, player.z + 2.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	gluSphere(qobj, 0.8, 50, 50);

	glBindVertexArray(playerVAO);
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
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- 셰이더 프로그램 활성화
	glUseProgram(shaderProgramID);

	// 그리기
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc);
	drawPlayer(modelLoc);
	drawEnemy(modelLoc);
	drawBuliding(modelLoc);
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
		-50.0f, 0.0f, -50.0f,//   0.5f, 0.5f, 0.5f, // Bottom-left
		 50.0f, 0.0f, -50.0f,//   0.5f, 0.5f, 0.5f, // Bottom-right
		-50.0f, 0.0f,  50.0f,//   0.5f, 0.5f, 0.5f, // Top-left
		 50.0f, 0.0f,  50.0f,//   0.5f, 0.5f, 0.5f  // Top-right
	};

	// 바닥의 인덱스 데이터
	GLuint floorIndices[] = {
		0, 1, 2, // 첫 번째 삼각형
		1, 3, 2  // 두 번째 삼각형
	};

	// VAO 생성 및 바인딩
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glGenBuffers(1, &floorEBO);

	glBindVertexArray(floorVAO);

	// VBO에 데이터 업로드
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// EBO에 데이터 업로드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

	// 위치 속성 지정 (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO 언바인딩
	glBindVertexArray(0);
}

void InitPlayer() {
	// 플레이어의 VAO 생성
	glGenVertexArrays(1, &playerVAO);
	glBindVertexArray(playerVAO);

	// 플레이어 모델용 Quadric 생성
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);

	// 필요하면 추가 데이터 생성 및 업로드
	// 현재는 gluCylinder와 gluSphere를 사용하므로 별도 VBO/EBO가 필요하지 않을 수 있습니다.

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
	float result = (float)((float)rand() / RAND_MAX) * (seed * 11) - (seed * 5.5f);

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
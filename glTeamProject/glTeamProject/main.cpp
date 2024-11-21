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

#define MAX_LINE_LENGTH 100
#define M_PI 3.141592
#define WINDOW_X 800
#define WINDOW_Y 600

using namespace glm;

//--- 아래 5개 함수는 사용자 정의 함수
void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void InitFloor();
void InitPlayer(const char* objFilename);
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

//--- obj파일 읽기
typedef struct {
	float x, y, z;
} Vertex;

typedef struct {
	unsigned int v1, v2, v3;
} Face;

typedef struct {
	Vertex* vertices;
	size_t vertex_count;
	Face* faces;
	size_t face_count;
} Model;
Model playerModel;

void read_obj_file(const char* filename, Model* model) {
	FILE* file;
	fopen_s(&file, filename, "r");
	if (!file) {
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}

	char line[MAX_LINE_LENGTH];
	model->vertex_count = 0;
	model->face_count = 0;

	// 1단계: 정점(Vertex)와 면(Face)의 개수를 계산
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			model->vertex_count++;
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			model->face_count++;
		}
	}

	// 메모리 할당
	model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
	model->faces = (Face*)malloc(model->face_count * sizeof(Face));

	// 2단계: 파일을 다시 읽으며 데이터 파싱
	fseek(file, 0, SEEK_SET); // 파일 포인터를 처음으로 이동
	size_t vertex_index = 0;
	size_t face_index = 0;

	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// 정점(Vertex) 데이터 읽기
			sscanf_s(line + 2, "%f %f %f",
				&model->vertices[vertex_index].x,
				&model->vertices[vertex_index].y,
				&model->vertices[vertex_index].z);
			vertex_index++;
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// 면(Face) 데이터 읽기
			unsigned int v1, v2, v3;
			int matches = sscanf_s(line + 2, "%u//%*u %u//%*u %u//%*u", &v1, &v2, &v3);

			// '//' 형식이 없는 경우 처리
			if (matches != 3) {
				sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);
			}

			model->faces[face_index].v1 = v1 - 1; // OBJ 파일의 인덱스는 1부터 시작하므로 1을 뺌
			model->faces[face_index].v2 = v2 - 1;
			model->faces[face_index].v3 = v3 - 1;
			face_index++;
		}
	}

	for (size_t i = 0; i < playerModel.vertex_count; ++i) {
		printf("Vertex %zu: %f %f %f\n", i, playerModel.vertices[i].x, playerModel.vertices[i].y, playerModel.vertices[i].z);
	}
	for (size_t i = 0; i < playerModel.face_count; ++i) {
		printf("Face %zu: %u %u %u\n", i, playerModel.faces[i].v1, playerModel.faces[i].v2, playerModel.faces[i].v3);
	}
	fclose(file);
}

//--- 셰이더 변수 선언
GLint width, height;
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
GLuint vertexShader, fragmentShader; //--- 세이더 객체
GLuint shaderProgramID; //--- 셰이더 프로그램
GLuint floorVAO, floorVBO, floorEBO;
GLuint playerVAO, playerVBO, playerEBO;
mat4 view;
mat4 projection;
//--- 전역 변수 선언
GLfloat mouseX = 0.0f, mouseY = 0.0f;

vec3 cameraPos = vec3(8.5f, 3.0f, 0.0f);		//--- 카메라 위치
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- 카메라 바라보는 방향
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- 카메라 위쪽 방향
float cameraAngle = 0.0f;						// 카메라의 현재 회전 각도 (초기값)
float rotationSpeed = 1.0f;						// 카메라 회전 속도 (deg/sec)

bool pressedY = false;
bool pressedR = false;
bool pressedA = false;

void setupCamera() {
	if (pressedA) {
		float radiusX = 8.5f;
		float radiusY = 3.0f;

		// 각도에 따른 카메라 방향을 설정
		cameraDirection.x = radiusX * cos(glm::radians(cameraAngle)) + radiusX;
		cameraDirection.y = radiusY * sin(glm::radians(cameraAngle)) + radiusY;
	}
	if (pressedY) {
		float radius = 8.5f; // 반지름 (원의 중심에서 카메라의 위치까지의 거리)

		// 각도에 따른 카메라 방향을 설정
		cameraDirection.x = radius * cos(glm::radians(cameraAngle)) + radius;
		cameraDirection.z = radius * sin(glm::radians(cameraAngle));
	}
	if (pressedR) {
		cameraPos.x = 8.5f * cos(glm::radians(cameraAngle));
		cameraPos.z = 8.5f * sin(glm::radians(cameraAngle));
	}

	view = lookAt(cameraPos, cameraDirection, cameraUp);
	projection = perspective(radians(45.0f), (float)WINDOW_X / (float)WINDOW_Y, 0.1f, 50.0f);
}

// 타이머 함수
void timerFunc(int value) {
	if (pressedR || pressedY || pressedA) {
		cameraAngle += rotationSpeed;
		if (cameraAngle >= 360.0f) cameraAngle -= 360.0f;
		setupCamera();
	}
	glutPostRedisplay();
	glutTimerFunc(100, timerFunc, 0);
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'x':
		cameraPos += vec3(0.0f, 0.0f, 0.5f);
		cameraDirection += vec3(0.0f, 0.0f, 0.5f);
		setupCamera();
		break;
	case 'X':
		cameraPos -= vec3(0.0f, 0.0f, 0.5f);
		cameraDirection -= vec3(0.0f, 0.0f, 0.5f);
		setupCamera();
		break;
	case 'z':
		cameraPos += vec3(0.5f, 0.0f, 0.0f);
		cameraDirection += vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'Z':
		cameraPos -= vec3(0.5f, 0.0f, 0.0f);
		cameraDirection -= vec3(0.5f, 0.0f, 0.0f);
		setupCamera();
		break;
	case 'y':
		pressedY = !pressedY;
		cameraAngle = -180;
		break;
	case 'r':
		pressedR = !pressedR;
		cameraPos = vec3(8.5f, 3.0f, 0.0f);
		cameraAngle = 0;
		break;
	case 'a':
		pressedA = !pressedA;
		cameraAngle = -180;
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
	mouseX = (2.0f * x / WINDOW_X) - 1.0f;
	mouseY = 1.0f - (2.0f * y / WINDOW_Y);
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

//--- 메인 함수
void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	srand(NULL);
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
	InitPlayer("obj.obj");
	InitFloor();
	setupCamera();
	glEnable(GL_DEPTH_TEST);

	//--- 세이더 프로그램 만들기
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
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
	glBindVertexArray(playerVAO);

	mat4 playerModelMat = mat4(1.0f); // 플레이어 모델 행렬
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.8f, 0.5f);
	glDrawElements(GL_TRIANGLES, playerModel.face_count * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // VAO 언바인딩
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
		-5.0f, 0.0f, -5.0f,//   0.5f, 0.5f, 0.5f, // Bottom-left
		 5.0f, 0.0f, -5.0f,//   0.5f, 0.5f, 0.5f, // Bottom-right
		-5.0f, 0.0f,  5.0f,//   0.5f, 0.5f, 0.5f, // Top-left
		 5.0f, 0.0f,  5.0f,//   0.5f, 0.5f, 0.5f  // Top-right
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

void InitPlayer(const char* objFilename) {
	read_obj_file(objFilename, &playerModel);

	// VAO, VBO, EBO 생성
	glGenVertexArrays(1, &playerVAO);
	glGenBuffers(1, &playerVBO);
	glGenBuffers(1, &playerEBO);

	glBindVertexArray(playerVAO);

	// VBO에 정점 데이터 업로드
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
	glBufferData(GL_ARRAY_BUFFER, playerModel.vertex_count * sizeof(Vertex), playerModel.vertices, GL_STATIC_DRAW);

	// EBO에 인덱스 데이터 업로드
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, playerModel.face_count * sizeof(Face), playerModel.faces, GL_STATIC_DRAW);

	// 정점 속성 설정 (attribute 0: position)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // VAO 언바인딩
}

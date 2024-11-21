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

//--- �Ʒ� 5�� �Լ��� ����� ���� �Լ�
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
		perror("ERROR: ���̴� ������ �� �� �����ϴ�.");
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

//--- obj���� �б�
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

	// 1�ܰ�: ����(Vertex)�� ��(Face)�� ������ ���
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			model->vertex_count++;
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			model->face_count++;
		}
	}

	// �޸� �Ҵ�
	model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
	model->faces = (Face*)malloc(model->face_count * sizeof(Face));

	// 2�ܰ�: ������ �ٽ� ������ ������ �Ľ�
	fseek(file, 0, SEEK_SET); // ���� �����͸� ó������ �̵�
	size_t vertex_index = 0;
	size_t face_index = 0;

	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// ����(Vertex) ������ �б�
			sscanf_s(line + 2, "%f %f %f",
				&model->vertices[vertex_index].x,
				&model->vertices[vertex_index].y,
				&model->vertices[vertex_index].z);
			vertex_index++;
		}
		else if (line[0] == 'f' && line[1] == ' ') {
			// ��(Face) ������ �б�
			unsigned int v1, v2, v3;
			int matches = sscanf_s(line + 2, "%u//%*u %u//%*u %u//%*u", &v1, &v2, &v3);

			// '//' ������ ���� ��� ó��
			if (matches != 3) {
				sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);
			}

			model->faces[face_index].v1 = v1 - 1; // OBJ ������ �ε����� 1���� �����ϹǷ� 1�� ��
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

//--- ���̴� ���� ����
GLint width, height;
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
GLuint shaderProgramID; //--- ���̴� ���α׷�
GLuint floorVAO, floorVBO, floorEBO;
GLuint playerVAO, playerVBO, playerEBO;
mat4 view;
mat4 projection;
//--- ���� ���� ����
GLfloat mouseX = 0.0f, mouseY = 0.0f;

vec3 cameraPos = vec3(8.5f, 3.0f, 0.0f);		//--- ī�޶� ��ġ
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f);	//--- ī�޶� �ٶ󺸴� ����
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);			//--- ī�޶� ���� ����
float cameraAngle = 0.0f;						// ī�޶��� ���� ȸ�� ���� (�ʱⰪ)
float rotationSpeed = 1.0f;						// ī�޶� ȸ�� �ӵ� (deg/sec)

bool pressedY = false;
bool pressedR = false;
bool pressedA = false;

void setupCamera() {
	if (pressedA) {
		float radiusX = 8.5f;
		float radiusY = 3.0f;

		// ������ ���� ī�޶� ������ ����
		cameraDirection.x = radiusX * cos(glm::radians(cameraAngle)) + radiusX;
		cameraDirection.y = radiusY * sin(glm::radians(cameraAngle)) + radiusY;
	}
	if (pressedY) {
		float radius = 8.5f; // ������ (���� �߽ɿ��� ī�޶��� ��ġ������ �Ÿ�)

		// ������ ���� ī�޶� ������ ����
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

// Ÿ�̸� �Լ�
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

//--- ���� �Լ�
void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	srand(NULL);
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
	InitPlayer("obj.obj");
	InitFloor();
	setupCamera();
	glEnable(GL_DEPTH_TEST);

	//--- ���̴� ���α׷� �����
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	glutTimerFunc(1000, timerFunc, 0);
	glutMainLoop();
}

//--- ���ؽ� ���̴� ��ü �����
void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	if (!vertexSource) {
		std::cerr << "ERROR: vertex shader ������ �ҷ����� ���߽��ϴ�." << std::endl;
	}
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//checkCompileErrors(vertexShader, "VERTEX");
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

//--- �����׸�Ʈ ���̴� ��ü �����
void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	if (!fragmentSource) {
		std::cerr << "ERROR: fragment shader ������ �ҷ����� ���߽��ϴ�." << std::endl;
	}
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

//--- ���̴� ���α׷� ����� ���̴� ��ü ��ũ�ϱ�
void make_shaderProgram()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����

	//-- shader Program
	shaderProgramID = glCreateProgram();

	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);


	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);

	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
}

void drawFloor(GLint modelLoc) {
	// �ٴ�
	glBindVertexArray(floorVAO);
	mat4 floorModel = mat4(1.0f); // �� ��� (��ȯ ����)
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(floorModel));

	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.8f, 0.5f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void drawPlayer(GLint modelLoc) {
	glBindVertexArray(playerVAO);

	mat4 playerModelMat = mat4(1.0f); // �÷��̾� �� ���
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.8f, 0.5f);
	glDrawElements(GL_TRIANGLES, playerModel.face_count * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // VAO ����ε�
}

//--- ��� �ݹ� �Լ�
GLvoid drawScene() {
	//--- ���� ���� �� ���� Ŭ����
	glClearColor(1.0, 1.0, 1.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//--- ���̴� ���α׷� Ȱ��ȭ
	glUseProgram(shaderProgramID);

	// �׸���
	GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
	GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));

	drawFloor(modelLoc);
	drawPlayer(modelLoc);
	
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
		-5.0f, 0.0f, -5.0f,//   0.5f, 0.5f, 0.5f, // Bottom-left
		 5.0f, 0.0f, -5.0f,//   0.5f, 0.5f, 0.5f, // Bottom-right
		-5.0f, 0.0f,  5.0f,//   0.5f, 0.5f, 0.5f, // Top-left
		 5.0f, 0.0f,  5.0f,//   0.5f, 0.5f, 0.5f  // Top-right
	};

	// �ٴ��� �ε��� ������
	GLuint floorIndices[] = {
		0, 1, 2, // ù ��° �ﰢ��
		1, 3, 2  // �� ��° �ﰢ��
	};

	// VAO ���� �� ���ε�
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glGenBuffers(1, &floorEBO);

	glBindVertexArray(floorVAO);

	// VBO�� ������ ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

	// EBO�� ������ ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floorIndices), floorIndices, GL_STATIC_DRAW);

	// ��ġ �Ӽ� ���� (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO ����ε�
	glBindVertexArray(0);
}

void InitPlayer(const char* objFilename) {
	read_obj_file(objFilename, &playerModel);

	// VAO, VBO, EBO ����
	glGenVertexArrays(1, &playerVAO);
	glGenBuffers(1, &playerVBO);
	glGenBuffers(1, &playerEBO);

	glBindVertexArray(playerVAO);

	// VBO�� ���� ������ ���ε�
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
	glBufferData(GL_ARRAY_BUFFER, playerModel.vertex_count * sizeof(Vertex), playerModel.vertices, GL_STATIC_DRAW);

	// EBO�� �ε��� ������ ���ε�
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, playerEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, playerModel.face_count * sizeof(Face), playerModel.faces, GL_STATIC_DRAW);

	// ���� �Ӽ� ���� (attribute 0: position)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0); // VAO ����ε�
}

#pragma once

#include <iostream>
#include <GL/glew.h>

extern GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����
extern GLuint vertexShader, fragmentShader; //--- ���̴� ��ü
extern GLuint shaderProgramID; //--- ���̴� ���α׷�

void make_vertexShaders();

void make_fragmentShaders();

void make_shaderProgram();

char* filetobuf(const char* file);

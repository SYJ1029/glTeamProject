#pragma once

#include <iostream>
#include <GL/glew.h>

extern GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수
extern GLuint vertexShader, fragmentShader; //--- 세이더 객체
extern GLuint shaderProgramID; //--- 셰이더 프로그램

void make_vertexShaders();

void make_fragmentShaders();

void make_shaderProgram();

char* filetobuf(const char* file);

#pragma once

#include <iostream>

#define MAX_LINE_LENGTH 100
#define M_PI 3.141592
#define WINDOW_X 800
#define WINDOW_Y 600

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

void read_obj_file(const char* filename, Model* model);
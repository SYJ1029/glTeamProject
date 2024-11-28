#pragma once


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
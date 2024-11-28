#pragma once


#include "readObj.h"

#define MAX_LINE_LENGTH 100
#define M_PI 3.141592
#define WINDOW_X 800
#define WINDOW_Y 600

using namespace glm;




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
	model->vertices = (GLfloat**)malloc(model->vertex_count * sizeof(Vertex));
	// vertices를 2차원 배열로 할당하게 했으므로 재할당 과정을 2번에 걸쳐서 해야함
	for (int i = 0; i < model->vertex_count; i++) {
		model->vertices[i] = (GLfloat*)malloc(3 * sizeof(GLfloat));
	}
	model->faces = (Face*)malloc(model->face_count * sizeof(Face));

	// 2단계: 파일을 다시 읽으며 데이터 파싱
	fseek(file, 0, SEEK_SET); // 파일 포인터를 처음으로 이동
	size_t vertex_index = 0;
	size_t face_index = 0;

	while (fgets(line, sizeof(line), file)) {
		if (line[0] == 'v' && line[1] == ' ') {
			// 정점(Vertex) 데이터 읽기
			sscanf_s(line + 2, "%f %f %f",
				&model->vertices[vertex_index][0],
				&model->vertices[vertex_index][1],
				&model->vertices[vertex_index][2]);
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

	for (size_t i = 0; i < model->vertex_count; ++i) {
		printf("Vertex %zu: %f %f %f\n", i, model->vertices[i][0], model->vertices[i][1], model->vertices[i][2]);
	}
	for (size_t i = 0; i < model->face_count; ++i) {
		printf("Face %zu: %u %u %u\n", i, model->faces[i].v1, model->faces[i].v2, model->faces[i].v3);
	}
	fclose(file);
}
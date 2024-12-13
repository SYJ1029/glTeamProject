#pragma once

#include "Building.h"
#include "readObj.h"
#include "rwTile.h"

GLuint buildVAO, buildVBO, buildEBO;



void InitBuliding(const char* objFilename, int** maptile, int& tilerow, int& tilecolumn, int numBuild, 
	Model* buildingModel, std::vector<Building>& g_buildings) {
	maptile = InitTileArr(maptile, tilerow, tilecolumn);

	for (int i = 0; i < numBuild; i++) {
		SetTile(maptile, tilerow, tilecolumn);
	}


	Building building;

	read_obj_file(objFilename, buildingModel);

	for (int i = 0; i < tilerow; i++) {
		for (int j = 0; j < tilecolumn; j++) {
			if (maptile[i][j] > 0) {
				building = { (float)i * 5 - 100.0f, 0.0f, (float)j * 5 - 100.0f, 5.0f, 10.0f, 5.0f };
				g_buildings.push_back(building);
			}
		}
	}




	glGenVertexArrays(1, &buildVAO);
	glGenBuffers(1, &buildVBO);
	glGenBuffers(1, &buildEBO);

	glBindVertexArray(buildVAO);

	//VBO에 데이터 등록
	glBindBuffer(GL_ARRAY_BUFFER, buildVBO);
	glBufferData(GL_ARRAY_BUFFER, 40 * sizeof(buildingModel->vertices), buildingModel->vertices, GL_STATIC_DRAW);
	cout << sizeof(buildingModel->vertices) << endl;

	//EBO에 데이터 등록
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buildEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 40 * sizeof(buildingModel->faces), buildingModel->faces, GL_STATIC_DRAW);
	cout << sizeof(buildingModel->faces) << endl;


	// 위치 속성 지정 (attribute 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// VAO 언바인딩
	glBindVertexArray(0);
}



void ConcatenateTile(int index, std::vector<Building>& g_buildings, float dx, float dz) {
	float clipx = abs(dx) - 50.0f;
	float clipz = abs(dz) - 50.0f;

	if (clipx > 0 && abs(g_buildings[index].x) < 100.0f - clipx) { // floor가 클리핑한 범위가 Tile의 범위를 벗어남
		if (dx < 0) {
			// player.x < 0 은 좌측으로 잘렸음을 의미

			g_buildings[index].x -= 100.0f; // 끝으로 이동한다.
		}
		else { // player.x = 0인 경우는 고려할 필요가 없음.
			g_buildings[index].x += 100.0f; // 반대 끝으로 이동한다.
		}
	}

	if (clipz > 0 && abs(g_buildings[index].z) < 100.0f - clipz) { // 벗어남 2
		if (dz < 0) {
			// player.z< 0 은 위쪽으로 잘렸음을 의미

			g_buildings[index].z -= 100.0f;
		}
		else {
			g_buildings[index].z += 100.0f;
		}
	}
}

void drawBuliding(GLint modelLoc, std::vector<Building>& g_buildings, float dx, float dz) {
	glBindVertexArray(buildVAO);
	for (int i = 0; i < g_buildings.size(); i++) {
		ConcatenateTile(i, g_buildings,  dx, dz);
		if (g_buildings[i].x >= -50.0f + dx && g_buildings[i].x <= 50.0f + dx &&
			g_buildings[i].z >= -50.0f + dz && g_buildings[i].z <= 50.0f + dz) {
			mat4 buildingModelMat = mat4(1.0f);
			buildingModelMat = translate(buildingModelMat, vec3(g_buildings[i].x, g_buildings[i].y, g_buildings[i].z));
			buildingModelMat = scale(buildingModelMat, vec3(g_buildings[i].scale.x, g_buildings[i].scale.y, g_buildings[i].scale.z));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(buildingModelMat));
			glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 1.0f);

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

	glBindVertexArray(0);
}
#pragma once

#include "Building.h"
#include "readObj.h"
#include "rwTile.h"
#include "player.h"
#include "Enemy.h"
#include "Collision.h"




void InitBuliding(const char* objFilename, int** maptile, int& tilerow, int& tilecolumn, int numBuild, 
	std::vector<Building>& g_buildings) {
	maptile = InitTileArr(maptile, tilerow, tilecolumn);

	for (int i = 0; i < numBuild; i++) {
		SetTile(maptile, tilerow, tilecolumn);
	}


	Building building;
	Model* buildingModel = new Model;
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



void ConcatenateTile(int index, std::vector<Building>& g_buildings, float dx, float dz, int** maptile, int row, int column) {
	float clipx = abs(dx) - 50.0f;
	float clipz = abs(dz) - 50.0f;

	if (clipx > 0 && abs(g_buildings[index].x) < 100.0f - clipx) { // floor가 클리핑한 범위가 Tile의 범위를 벗어남
		if (dx < 0) {
			// player.x < 0 은 좌측으로 잘렸음을 의미

			g_buildings[index].x -= 100.0f; // 끝으로 이동한다.
			ShiftTileMatrix(maptile, -1, row, column);
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

void drawBuliding(GLint modelLoc, std::vector<Building>& g_buildings, float dx, float dz, int** maptile, int row, int column, bool nvd) {
	glBindVertexArray(buildVAO);
	glUniform1i(glGetUniformLocation(shaderProgramID, "objtype"), 2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < g_buildings.size(); i++) {
		//ConcatenateTile(i, g_buildings,  dx, dz, maptile, row, column);
		if (g_buildings[i].x >= -50.0f + dx && g_buildings[i].x <= 50.0f + dx &&
			g_buildings[i].z >= -50.0f + dz && g_buildings[i].z <= 50.0f + dz) {
			mat4 buildingModelMat = mat4(1.0f);
			buildingModelMat = translate(buildingModelMat, vec3(g_buildings[i].x, g_buildings[i].y, g_buildings[i].z));
			buildingModelMat = scale(buildingModelMat, vec3(g_buildings[i].scale.x, g_buildings[i].scale.y, g_buildings[i].scale.z));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(buildingModelMat));
			glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 1.0f);

			if(nvd)
				glUniform1f(glGetUniformLocation(shaderProgramID, "buildalpha"), 0.2f);
			else
				glUniform1f(glGetUniformLocation(shaderProgramID, "buildalpha"), 1.0f);
		}
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}


	glUniform1i(glGetUniformLocation(shaderProgramID, "objtype"), 0);
	glDisable(GL_BLEND);
	glBindVertexArray(0);
}


bool BuildingCollisionPlayer(std::vector<Building>& g_buildings, Player& player) {
	glm::vec3 pos1, pos2;
	for (int i = 0; i < g_buildings.size(); i++) {
		pos1 = { g_buildings[i].x, 0.0f, g_buildings[i].z};
		pos2 = { g_buildings[i].x + g_buildings[i].scale.x, 0.0f, g_buildings[i].z + g_buildings[i].scale.z};

		if (CollisionCheckRect({ GetNextXZ(player, 0), player.y, GetNextXZ(player, 2) }, pos1, pos2)) {
			return true;
		}
	}

	return false;
}

bool BuildingCollisionBullet(std::vector<Building>& g_buildings, std::vector<Bullet>& g_bullets) {
	glm::vec3 pos1, pos2;
	std::vector<int> erasebulletlist;
	for (int i = 0; i < g_buildings.size(); i++) {
		pos1 = { g_buildings[i].x, 0.0f, g_buildings[i].z };
		pos2 = { g_buildings[i].x + g_buildings[i].scale.x, 0.0f, g_buildings[i].z + g_buildings[i].scale.z };
		for (int j = 0; j < g_bullets.size(); j++) {
			if (CollisionCheckRect({g_bullets[j].x, g_bullets[j].y, g_bullets[j].z}, pos1, pos2)) {
				erasebulletlist.push_back(j);
				break; // 충돌했다면 더 이상 탐색할 필요가 없음
			}
		}
	}

	int subcount = 0;

	for (int i = 0; i < erasebulletlist.size(); i++) {
 		g_bullets.erase(g_bullets.begin() + erasebulletlist[i] - subcount);
		subcount++;
	}

	erasebulletlist.clear();
	return false;
}


bool BuildingCollisionEnemy(std::vector<Building>& g_buildings, std::vector<Enemy>& g_enemies) {

	return false;
}
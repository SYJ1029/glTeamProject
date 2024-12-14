#include "player.h"
#include "Building.h"

void InitPlayer(GLUquadricObj* &qobj, Player &player) {
	// 플레이어 모델용 Quadric 생성
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);

	glBindVertexArray(0); // VAO 언바인딩
	player.x = 0.0f;
	player.y = 0.0f;
	player.z = 0.0f;
	player.dx = 0.0f;
	player.dy = 0.0f;
	player.dz = 0.0f;
	player.angleXZ = 0.0f;
	player.angleY = 0.0f;
	player.gun = true;
}

void drawPlayer(GLint modelLoc, GLUquadricObj*& qobj, Player &player) {
	glBindVertexArray(sphereVAO);

	mat4 playerModelMat = mat4(1.0f); // 플레이어 모델 행렬
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.0f, 0.0f);

	playerModelMat = glm::translate(playerModelMat, vec3(player.x, player.y + 2.0f, player.z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	playerModelMat = glm::rotate(playerModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	playerModelMat = glm::translate(playerModelMat, vec3(0.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	gluCylinder(qobj, 1.0, 0.3, 1.5, 20, 8);

	glBindVertexArray(hexVao);
	float radius = 1.8f;
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.2f, 0.2f, 0.2f);
	//손잡이
	mat4 gumModelHandle = playerModelMat;
	gumModelHandle = glm::translate(gumModelHandle, vec3(0.0f, 0.0f, 1.7f));
	gumModelHandle = glm::rotate(gumModelHandle, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	gumModelHandle = translate(gumModelHandle, vec3(radius * glm::cos(radians(player.angleXZ + 8.0f)), 0.0f, radius * glm::sin(radians(player.angleXZ + 8.0f))));
	gumModelHandle = glm::rotate(gumModelHandle, glm::radians(-player.angleXZ), vec3(0.0f, 1.0f, 0.0f));
	gumModelHandle = scale(gumModelHandle, vec3(0.1f, 0.1f, 0.1f));
	gumModelHandle = glm::translate(gumModelHandle, vec3(-1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(gumModelHandle));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//몸
	mat4 gumModelBody = playerModelMat;
	gumModelBody = glm::translate(gumModelBody, vec3(0.0f, 0.0f, 1.8f));
	gumModelBody = glm::rotate(gumModelBody, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	gumModelBody = translate(gumModelBody, vec3(radius * glm::cos(radians(player.angleXZ + 8.0f)), 0.0f, radius * glm::sin(radians(player.angleXZ + 8.0f))));
	gumModelBody = glm::rotate(gumModelBody, glm::radians(-player.angleXZ), vec3(0.0f, 1.0f, 0.0f));
	gumModelBody = scale(gumModelBody, vec3(0.1f, 0.1f, 0.1f));
	gumModelBody = glm::translate(gumModelBody, vec3(-1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(gumModelBody));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	// 총구
	if (!player.gun) {
		mat4 gumModelShoot = playerModelMat;
		gumModelShoot = glm::translate(gumModelShoot, vec3(0.0f, 0.0f, 1.8f));
		gumModelShoot = glm::rotate(gumModelShoot, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		gumModelShoot = translate(gumModelShoot, vec3(radius * glm::cos(radians(player.angleXZ + 8.0f)), 0.0f, radius * glm::sin(radians(player.angleXZ + 8.0f))));
		gumModelShoot = glm::rotate(gumModelShoot, glm::radians(-player.angleXZ), vec3(0.0f, 1.0f, 0.0f));
		gumModelShoot = scale(gumModelShoot, vec3(0.2f, 0.1f, 0.1f));
		gumModelShoot = glm::translate(gumModelShoot, vec3(0.26f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(gumModelShoot));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else {
		glBindVertexArray(pyramidVao);
		mat4 gumModelShoot = playerModelMat;
		gumModelShoot = glm::translate(gumModelShoot, vec3(0.0f, 0.0f, 1.8f));
		gumModelShoot = glm::rotate(gumModelShoot, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		gumModelShoot = translate(gumModelShoot, vec3(radius * glm::cos(radians(player.angleXZ + 8.0f)), 0.0f, radius * glm::sin(radians(player.angleXZ + 8.0f))));
		gumModelShoot = glm::rotate(gumModelShoot, glm::radians(-player.angleXZ), vec3(0.0f, 1.0f, 0.0f));
		gumModelShoot = scale(gumModelShoot, vec3(0.5f, 0.1f, 0.1f));
		gumModelShoot = glm::translate(gumModelShoot, vec3(0.4f, 0.0f, 0.0f));
		gumModelShoot = glm::rotate(gumModelShoot, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(gumModelShoot));
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0); // VAO 언바인딩
}

void applyGravity(Player& player) {
	if (player.dy > 0.0f) {
		player.dy -= 0.025f;
	}
	else {
		player.dy -= 0.035f;
	}
}

void jump(Player& player) {
	player.dy += 0.3f;
}

void updatePlayer(Player& player, bool buildcollision) {
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)),
		0.0f,
		sin(glm::radians(player.angleXZ))
	));

	if (buildcollision == false) {
		player.x += player.dx * direction.x - player.dz * direction.z;
		player.z += player.dx * direction.z + player.dz * direction.x;
	}

	if (player.y > 0.0f || player.dy < 0.0f || player.dy > 0.0f) {
		printf("%f, %f\n", player.y, player.dy);
		player.y += player.dy;

		applyGravity(player);
		if (player.y <= 0.0f) {
			player.y = 0.0f;
			player.dy = 0.0f;
		}
	}
}

float GetNextXZ(Player& player, int option) {
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)),
		0.0f,
		sin(glm::radians(player.angleXZ))
	));

	switch (option) {
	case 0:
		return player.x + player.dx * direction.x - player.dz * direction.z;
	case 1:
		return player.y;
	case 2:
		return player.z + player.dx * direction.z + player.dz * direction.x;
	}

}

void playerCollisionWithEnemy(Player& player, std::vector<Enemy>& g_enemies) {
	float playerRadius = 1.0f;
	float enemyRadius = 1.0f;
	float collisionDistance = playerRadius + enemyRadius + 1.0f;

	for (auto it = g_enemies.begin(); it != g_enemies.end(); ) {
		Enemy& enemy = *it;

		float dx = abs(player.x - it->x);
		float dz = abs(player.z - it->z);
		float distance = sqrt(dx * dx + dz * dz);

		if (distance < collisionDistance) {
			it = g_enemies.erase(it);
			// 일단은 지움 로직 추가해야함.
		}
		else {
			++it;
		}
	}
}

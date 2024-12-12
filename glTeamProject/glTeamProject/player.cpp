#include "player.h"

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

	mat4 gumModelBody = playerModelMat;
	gumModelBody = glm::translate(gumModelBody, vec3(0.0f, 0.0f, 1.8f));
	gumModelBody = glm::rotate(gumModelBody, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	gumModelBody = translate(gumModelBody, vec3(radius * glm::cos(radians(player.angleXZ + 8.0f)), 0.0f, radius * glm::sin(radians(player.angleXZ + 8.0f))));
	gumModelBody = glm::rotate(gumModelBody, glm::radians(-player.angleXZ), vec3(0.0f, 1.0f, 0.0f));
	gumModelBody = scale(gumModelBody, vec3(0.3f, 0.1f, 0.1f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(gumModelBody));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	mat4 gumModelHandle = playerModelMat;
	gumModelHandle = glm::translate(gumModelHandle, vec3(0.0f, 0.0f, 1.7f));
	gumModelHandle = glm::rotate(gumModelHandle, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	gumModelHandle = translate(gumModelHandle, vec3(radius * glm::cos(radians(player.angleXZ + 8.0f)), 0.0f, radius * glm::sin(radians(player.angleXZ + 8.0f))));
	gumModelHandle = glm::rotate(gumModelHandle, glm::radians(-player.angleXZ), vec3(0.0f, 1.0f, 0.0f));
	gumModelHandle = scale(gumModelHandle, vec3(0.1f, 0.1f, 0.1f));
	gumModelHandle = glm::translate(gumModelHandle, vec3(-1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(gumModelHandle));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0); // VAO 언바인딩
}

void applyGravity(Player &player){
	if (player.dy > 0.0f) {
		player.dy -= 0.4f;
	}
}

void jump(Player &player){
	player.dy += 2.0f;
}

void updatePlayer(Player &player) {
	vec3 direction = normalize(vec3(
		cos(glm::radians(player.angleXZ)),  // X축 성분
		0.0f,                              // Y축 (고정)
		sin(glm::radians(player.angleXZ))  // Z축 성분
	));
	player.x += player.dx * direction.x - player.dz * direction.z;
	player.z += player.dx * direction.z + player.dz * direction.x;

	applyGravity(player);
	if (player.y > 0.0f || player.dy > 0.0f) {
		player.y += player.dy;

		// 바닥에 도달했을 경우
		if (player.y < 0.0f) {
			player.y = 0.0f;
			player.dy = 0.0f; // 점프 상태 초기화
		}
	}
}

#include "player.h"

void InitPlayer(GLUquadricObj* &qobj, Player &player) {
	// �÷��̾� �𵨿� Quadric ����
	qobj = gluNewQuadric();
	gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricNormals(qobj, GLU_SMOOTH);
	gluQuadricOrientation(qobj, GLU_OUTSIDE);

	glBindVertexArray(0); // VAO ����ε�
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

	mat4 playerModelMat = mat4(1.0f); // �÷��̾� �� ���
	glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 0.0f, 0.0f);

	playerModelMat = glm::translate(playerModelMat, vec3(player.x, player.y + 2.0f, player.z));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	playerModelMat = glm::rotate(playerModelMat, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	playerModelMat = glm::translate(playerModelMat, vec3(0.0f, 0.0f, -2.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(playerModelMat));
	gluCylinder(qobj, 1.0, 0.3, 1.5, 20, 8);

	glBindVertexArray(0); // VAO ����ε�
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
		cos(glm::radians(player.angleXZ)),  // X�� ����
		0.0f,                              // Y�� (����)
		sin(glm::radians(player.angleXZ))  // Z�� ����
	));
	player.x += player.dx * direction.x - player.dz * direction.z;
	player.z += player.dx * direction.z + player.dz * direction.x;

	applyGravity(player);
	if (player.y > 0.0f || player.dy > 0.0f) {
		player.y += player.dy;

		// �ٴڿ� �������� ���
		if (player.y < 0.0f) {
			player.y = 0.0f;
			player.dy = 0.0f; // ���� ���� �ʱ�ȭ
		}
	}
}

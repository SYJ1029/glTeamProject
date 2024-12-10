#include "bullet.h"

void shootBullet(Player& player, std::vector<Bullet>& g_bullets) {
    Bullet newBullet;
    glm::vec3 direction = glm::normalize(vec3(
        cos(glm::radians(player.angleXZ)), // X축 방향
        0.0f,                              // Y축 (수평)
        sin(glm::radians(player.angleXZ))  // Z축 방향
    ));

    newBullet = { player.x, player.y + 1.95f, player.z };
    newBullet.dx = direction.x;
    newBullet.dy = direction.y;
    newBullet.dz = direction.z;
    newBullet.speed = 0.8f;

    g_bullets.push_back(newBullet);
}

void drawBullets(GLint modelLoc, Player& player, std::vector<Bullet>& g_bullets) {
    float radius = 0.1f;
    glBindVertexArray(sphereVAO);

    mat4 bulletModel;
    for (const Bullet& bullet : g_bullets) {
        bulletModel = mat4(1.0f);
        bulletModel = translate(bulletModel, vec3(bullet.x + radius * glm::cos(radians(player.angleXZ + 90.0f)), bullet.y, bullet.z + radius * glm::sin(radians(player.angleXZ + 90.0f))));
        bulletModel = scale(bulletModel, vec3(0.01f, 0.01f, 0.01f)); // 총알 크기 조정

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(bulletModel));
        glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 1.0f, 0.0f); // 노란색 총알

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void updateBullets(std::vector<Bullet>& g_bullets) {
    for (int i = 0; i < g_bullets.size(); i++) {
        g_bullets[i].x += g_bullets[i].dx * g_bullets[i].speed;
        g_bullets[i].y += g_bullets[i].dy * g_bullets[i].speed;
        g_bullets[i].z += g_bullets[i].dz * g_bullets[i].speed;

        if (g_bullets[i].x > 100.0f || g_bullets[i].x < -100.0f ||
            g_bullets[i].z > 100.0f || g_bullets[i].z < -100.0f) {
            g_bullets.erase(g_bullets.begin() + i);
            i--;
        }
    }
}
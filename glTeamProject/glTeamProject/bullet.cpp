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
    newBullet.speed = 2.0f;

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

void checkCollisionWithEnemies(std::vector<Bullet>& g_bullets, std::vector<Enemy>& g_enemies) {
    for (int i = 0; i < g_bullets.size(); i++) {
        Bullet& bullet = g_bullets[i];
        bool collided = false;

        // 가장 가까운 적과의 거리를 계산하기 위한 변수
        float minDistance = std::numeric_limits<float>::max();
        int closestEnemyIndex = -1;

        // 각 총알에 대해 모든 적과의 충돌을 검사
        for (int j = 0; j < g_enemies.size(); j++) {
            Enemy& enemy = g_enemies[j];
            float distanceToEnemy = glm::distance(glm::vec2(bullet.x, bullet.z), glm::vec2(enemy.x, enemy.z));

            // 총알의 다음 위치 계산
            float nextBulletX = bullet.x + bullet.dx * bullet.speed;
            float nextBulletZ = bullet.z + bullet.dz * bullet.speed;
            float distanceToNextBulletPos = glm::distance(glm::vec2(nextBulletX, nextBulletZ), glm::vec2(enemy.x, enemy.z));

            // 현재 위치와 다음 위치 사이의 거리 내에 적이 있는지 확인
            if (distanceToEnemy <= bullet.speed || distanceToNextBulletPos <= bullet.speed) {
                // 가장 가까운 적 찾기
                if (distanceToEnemy < minDistance) {
                    minDistance = distanceToEnemy;
                    closestEnemyIndex = j;
                }
            }
        }

        // 가장 가까운 적과 충돌이 발생했다면 해당 적을 제거
        if (closestEnemyIndex != -1) {
            g_enemies.erase(g_enemies.begin() + closestEnemyIndex);
            collided = true;
        }

        // 충돌이 발생했으면 해당 총알도 제거
        if (collided) {
            g_bullets.erase(g_bullets.begin() + i);
            i--;  // 인덱스 조정
        }
    }
}


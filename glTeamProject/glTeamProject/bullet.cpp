#include "bullet.h"

void shootBullet(Player& player, std::vector<Bullet>& g_bullets) {
    if (player.gun) {
        glm::vec3 direction = glm::normalize(vec3(
            cos(glm::radians(player.angleXZ)), // X축 방향
            0.0f,                              // Y축 (수평)
            sin(glm::radians(player.angleXZ))  // Z축 방향
        ));

        Bullet newBullet;
        newBullet = { player.x, player.y + 1.95f, player.z };
        newBullet.dx = direction.x;
        newBullet.dy = direction.y;
        newBullet.dz = direction.z;
        printf("%f, %f, %f, %f\n", newBullet.dx, newBullet.dz, newBullet.x, newBullet.z);

        g_bullets.push_back(newBullet);
    }
    else {
        int numBullets = 5; // 한 번에 발사할 총알의 수
        float spreadAngle = 10.0f; // 중심 각도로부터 최대 퍼짐 각도

        for (int i = 0; i < numBullets; i++) {
            float angleOffset = spreadAngle * ((float)i / (numBullets - 1) * 2.0f - 1.0f); // -10도에서 +10도로 변화
            glm::vec3 direction = glm::normalize(glm::vec3(
                cos(glm::radians(player.angleXZ + angleOffset)), // X축 방향 조정
                0.0f,                                            // Y축 (수평)
                sin(glm::radians(player.angleXZ + angleOffset))  // Z축 방향 조정
            ));

            Bullet newBullet;
            newBullet = { player.x, player.y + 1.95f, player.z };   
            newBullet.dx = direction.x;
            newBullet.dy = direction.y;
            newBullet.dz = direction.z;
            printf("%f, %f, %f, %f\n", newBullet.dx, newBullet.dz, newBullet.x, newBullet.z);

            g_bullets.push_back(newBullet);
        }
    }
}

void drawBullets(GLint modelLoc, Player& player, std::vector<Bullet>& g_bullets) {
    float radius = 1.5f;
    glBindVertexArray(sphereVAO);

    mat4 bulletModel;
    for (const Bullet& bullet : g_bullets) {
        bulletModel = mat4(1.0f);
        bulletModel = translate(bulletModel, vec3(bullet.x + radius * glm::cos(radians(player.angleXZ + 2.2f)), bullet.y, bullet.z + radius * glm::sin(radians(player.angleXZ + 2.2f))));
        bulletModel = scale(bulletModel, vec3(0.01f, 0.01f, 0.01f)); // 총알 크기 조정

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(bulletModel));
        glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 1.0f, 0.0f); // 노란색 총알

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void updateBullets(std::vector<Bullet>& g_bullets) {
    for (int i = 0; i < g_bullets.size(); i++) {
        g_bullets[i].x += g_bullets[i].dx;
        g_bullets[i].y += g_bullets[i].dy;
        g_bullets[i].z += g_bullets[i].dz;

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
            float nextBulletX = bullet.x + bullet.dx;
            float nextBulletZ = bullet.z + bullet.dz;
            float distanceToNextBulletPos = glm::distance(glm::vec2(nextBulletX, nextBulletZ), glm::vec2(enemy.x, enemy.z));
            float bulletMovedDistance = sqrt(bullet.dx * bullet.dx + bullet.dz * bullet.dz);

            // 현재 위치와 다음 위치 사이의 거리 내에 적이 있는지 확인
            if (distanceToEnemy <= bulletMovedDistance || distanceToNextBulletPos <= bulletMovedDistance) {
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


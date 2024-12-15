#include "bullet.h"

void shootBullet(Player& player, std::vector<Bullet>& g_bullets) {
    if (player.gun) {
        glm::vec3 direction = glm::normalize(glm::vec3(
            cos(glm::radians(player.angleXZ)), // X�� ����
            -(1.0f / 90.0f) * player.angleY,  // Y��
            sin(glm::radians(player.angleXZ))   // Z�� ����
        ));

        Bullet newBullet;
        newBullet = { player.x, player.y + 1.95f, player.z };
        newBullet.dx = direction.x;
        newBullet.dy = direction.y;
        newBullet.dz = direction.z;
        newBullet.damage = 2;
        printf("%f, %f, %f, %f\n", newBullet.dx, newBullet.dz, newBullet.x, newBullet.z);

        g_bullets.push_back(newBullet);
    }
    else {
        int numBullets = 5; // �� ���� �߻��� �Ѿ��� ��
        float spreadAngle = 10.0f; // �߽� �����κ��� �ִ� ���� ����

        for (int i = 0; i < numBullets; i++) {
            float angleOffset = spreadAngle * ((float)i / (numBullets - 1) * 2.0f - 1.0f); // -10������ +10���� ��ȭ
            glm::vec3 direction = glm::normalize(glm::vec3(
                cos(glm::radians(player.angleXZ + angleOffset)), // X�� ���� ����
                0.0f,                                            // Y�� (����)
                sin(glm::radians(player.angleXZ + angleOffset))  // Z�� ���� ����
            ));

            Bullet newBullet;
            newBullet = { player.x, player.y + 1.95f, player.z };   
            newBullet.dx = direction.x;
            newBullet.dy = direction.y;
            newBullet.dz = direction.z;
            newBullet.damage = 1;
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
        bulletModel = translate(bulletModel, vec3(bullet.x + radius * glm::cos(radians(player.angleXZ)), bullet.y  - (1.0f / 180.0f * player.angleY), bullet.z + radius * glm::sin(radians(player.angleXZ))));
        bulletModel = scale(bulletModel, vec3(0.01f, 0.01f, 0.01f)); // �Ѿ� ũ�� ����

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(bulletModel));
        glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 1.0f, 1.0f, 0.0f); // ����� �Ѿ�

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

void updateBullets(std::vector<Bullet>& g_bullets) {
    for (int i = 0; i < g_bullets.size(); i++) {
        g_bullets[i].x += g_bullets[i].dx;
        g_bullets[i].y += g_bullets[i].dy;
        g_bullets[i].z += g_bullets[i].dz;

        if (g_bullets[i].x > 50.0f || g_bullets[i].x < -50.0f ||
            g_bullets[i].z > 50.0f || g_bullets[i].z < -50.0f) {
            g_bullets.erase(g_bullets.begin() + i);
            i--;
        }
    }
}

void checkCollisionWithEnemies(std::vector<Bullet>& g_bullets, std::vector<Enemy>& g_enemies) {
    for (int i = 0; i < g_bullets.size(); i++) {
        Bullet& bullet = g_bullets[i];
        bool collided = false;

        // ���� ����� ������ �Ÿ��� ����ϱ� ���� ����
        float minDistance = std::numeric_limits<float>::max();
        int closestEnemyIndex = -1;

        // �� �Ѿ˿� ���� ��� ������ �浹�� �˻�
        for (int j = 0; j < g_enemies.size(); j++) {
            Enemy& enemy = g_enemies[j];
            float distanceToEnemy = glm::distance(glm::vec2(bullet.x, bullet.z), glm::vec2(enemy.x, enemy.z));

            // �Ѿ��� ���� ��ġ ���
            float nextBulletX = bullet.x + bullet.dx;
            float nextBulletZ = bullet.z + bullet.dz;
            float distanceToNextBulletPos = glm::distance(glm::vec2(nextBulletX, nextBulletZ), glm::vec2(enemy.x, enemy.z));
            float bulletMovedDistance = sqrt(bullet.dx * bullet.dx + bullet.dz * bullet.dz);

            // ���� ��ġ�� ���� ��ġ ������ �Ÿ� ���� ���� �ִ��� Ȯ��
            if (distanceToEnemy <= bulletMovedDistance || distanceToNextBulletPos <= bulletMovedDistance) {
                // ���� ����� �� ã��
                if (distanceToEnemy < minDistance) {
                    minDistance = distanceToEnemy;
                    closestEnemyIndex = j;
                }
            }
        }

        // ���� ����� ���� �浹�� �߻��ߴٸ� �ش� ���� ����
        if (closestEnemyIndex != -1) {
            // �����ؾ���
            g_enemies[closestEnemyIndex].hp -= g_bullets[i].damage;
            g_enemies[closestEnemyIndex].damaged = true;
 
            //g_enemies.erase(g_enemies.begin() + closestEnemyIndex);
            collided = true;
        }

        // �浹�� �߻������� �ش� �Ѿ˵� ����
        if (collided) {
            g_bullets.erase(g_bullets.begin() + i);
            i--;  // �ε��� ����
        }
    }
}


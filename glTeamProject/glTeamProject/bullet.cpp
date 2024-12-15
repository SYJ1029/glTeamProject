#include "bullet.h"

void shootBullet(Player& player, std::vector<Bullet>& g_bullets, glm::vec3 cameraDirection) {
    if (player.gun) {
        Bullet newBullet;
        newBullet = { player.gunMuzzleWorldPositionVec3.x, player.gunMuzzleWorldPositionVec3.y, player.gunMuzzleWorldPositionVec3.z };
        newBullet.dx = cameraDirection.x;
        newBullet.dy = cameraDirection.y;
        newBullet.dz = cameraDirection.z;
        newBullet.damage = 2;

        g_bullets.push_back(newBullet);
    }
    else if(!player.gun){
        int numBullets = 5; // �� ���� �߻��� �Ѿ��� ��
        float spreadAngle = 10.0f; // �߽� �����κ��� �ִ� ���� ����

        for (int i = 0; i < numBullets; i++) {
            float angleOffset = spreadAngle * ((float)i / (numBullets - 1) * 2.0f - 1.0f); // -10������ +10���� ��ȭ

            glm::vec3 spreadDirection = glm::normalize(glm::vec3(
                cameraDirection.x * cos(glm::radians(angleOffset)) - cameraDirection.z * sin(glm::radians(angleOffset)),
                cameraDirection.y,
                cameraDirection.x * sin(glm::radians(angleOffset)) + cameraDirection.z * cos(glm::radians(angleOffset))
            ));

            Bullet newBullet;
            newBullet = { player.gunMuzzleWorldPositionVec3.x, player.gunMuzzleWorldPositionVec3.y, player.gunMuzzleWorldPositionVec3.z };
            newBullet.dx = spreadDirection.x;
            newBullet.dy = spreadDirection.y;
            newBullet.dz = spreadDirection.z;
            newBullet.damage = 1;

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
        bulletModel = translate(bulletModel, vec3(bullet.x, bullet.y, bullet.z));
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


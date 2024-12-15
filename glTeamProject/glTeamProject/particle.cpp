#include "particle.h"

int maxParticles = 50;

std::vector<Particle> particles;

void addParticles(vec3 enemyPos) {
    float offset = 2.0 / maxParticles;
    float increment = M_PI * (3.0 - sqrt(5.0));  // 황금각

    for (int i = 0; i < maxParticles; i++) {
        Particle newParticle;
        float y = ((i * offset) - 1) + (offset / 2);
        float r = sqrt(1 - pow(y, 2));

        float phi = ((i + 1) % maxParticles) * increment;

        float x = cos(phi) * r;
        float z = sin(phi) * r;

        newParticle.position = enemyPos;  // 초기 위치
        newParticle.velocity = glm::vec3(x, y, z) * 1.0f;  // 방향 및 속도
        newParticle.color = glm::vec4(rand() % 256 / 255.0f, rand() % 256 / 255.0f, rand() % 256 / 255.0f, 1.0f);
        newParticle.life = 100;

        particles.push_back(newParticle);
    }
}

void updateParticles() {
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->position += it->velocity;
        if (it->life <= 0) {
            it = particles.erase(it);
        }
        else {
            ++it;  // 다음 파티클로 이동
        }
    }
}

void drawParticles(GLint modelLoc) {
    glBindVertexArray(sphereVAO);
    glUniform3f(glGetUniformLocation(shaderProgramID, "objectColor"), 0.0f, 0.0f, 0.0f);

    for (auto& particle : particles) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), particle.position);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}

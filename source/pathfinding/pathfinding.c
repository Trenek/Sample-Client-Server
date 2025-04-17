#include <cglm.h>

static void moveEnemy(vec3 player, vec3 enemy, float deltaTime) {
    static float r = 0;

    enemy[0] = player[0] + sin(r);
    enemy[1] = player[1] + cos(r);
    enemy[2] = 1;

    r += deltaTime;
}

void moveEnemies(vec3 player, size_t qEnemies, vec3 *enemies[qEnemies], float deltaTime) {
    while (qEnemies --> 0) {
        moveEnemy(player, *enemies[qEnemies], deltaTime);
    }
}

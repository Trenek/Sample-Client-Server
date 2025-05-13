#include <stdlib.h>

#include <openGJK.h>

typedef float vec3[3];

bool checkCubeCollision(vec3 shape3d1[8], vec3 shape3d2[8]) {
    gkPolytope p1 = {
        .numpoints = 8,
        .coord = malloc(8 * sizeof(gkFloat*))
    };

    gkPolytope p2 = {
        .numpoints = 8,
        .coord = malloc(8 * sizeof(gkFloat*))
    };

    gkSimplex s = {
        .nvrtx = 0
    };

    for (int i = 0; i < 8; i++) {
        p1.coord[i] = malloc(3 * sizeof(gkFloat));
        p1.coord[i][0] = shape3d1[i][0];
        p1.coord[i][1] = shape3d1[i][1];
        p1.coord[i][2] = shape3d1[i][2];

        p2.coord[i] = malloc(3 * sizeof(gkFloat));
        p2.coord[i][0] = shape3d2[i][0];
        p2.coord[i][1] = shape3d2[i][1];
        p2.coord[i][2] = shape3d2[i][2];
    }

    gkFloat distance = compute_minimum_distance(p1, p2, &s);

    for (int i = 0; i < 8; i++) {
        free(p1.coord[i]);
        free(p2.coord[i]);
    }
    free(p1.coord);
    free(p2.coord);

    return distance <= 0.0;
}

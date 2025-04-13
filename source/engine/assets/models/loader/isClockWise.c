#include <stdint.h>
#include <math.h>

#include "cglm.h"

float vec2_angle(vec2 a) {
    float c = glm_vec2_norm(a);

    float sin = a[1] / c;
    float cos = a[0] / c;

    float q = (
        sin >= 0 && cos >= 0 ? 0 :
        sin <= 0 && cos >= 0 ? 3 :
        sin >= 0 && cos <= 0 ? 1 :
     /* sin <= 0 && cos <= 0*/ 2
    );

    float asin = asinf(sin);

    return q == 1 ? M_PI - asin : 
           q == 0 ? asin : 
           q == 3 ? asin + 2 * M_PI :
                    M_PI - asin;
}

bool isInBetween(vec2 C, vec2 B, vec2 A, vec2 D) {
    float ba = vec2_angle((vec2){ A[0] - B[0], A[1] - B[1] });
    float bc = vec2_angle((vec2){ C[0] - B[0], C[1] - B[1] });
    float bd = vec2_angle((vec2){ D[0] - B[0], D[1] - B[1] });

    return (
        bc > ba ? bd > ba && bc > bd :
        bc < ba ? !(bd > bc && ba > bd) :
                  bd == ba && bd == bc
    );
}


float getAngle(vec2 A, vec2 B, vec2 C) {
    float ba = vec2_angle((vec2){ A[0] - B[0], A[1] - B[1] });
    float bc = vec2_angle((vec2){ C[0] - B[0], C[1] - B[1] });

    float result = ba - bc;

    if (result < 0) result += 2 * M_PI;

    return result;
}

float getAngle2(vec2 A, vec2 B, vec2 C) {
    float ba = vec2_angle((vec2){ A[0] - B[0], A[1] - B[1] });
    float bc = vec2_angle((vec2){ C[0] - B[0], C[1] - B[1] });

    float result = ba - bc;

    return result;
}

bool isCounterClockwise(size_t N, vec2 *poly) {
    double angle = 0;

    for (size_t i = 0; i < N; i += 1) {
        angle += getAngle(poly[(i - 1 + N) % N], poly[i], poly[(i + 1) % N]);
    }

    return fabs(angle - M_PI * (N - 2)) < 10e-5;
}

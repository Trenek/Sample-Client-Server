#include <math.h>
#include <stddef.h>

typedef float vec2[2];

float vec2_angle(vec2 a) {
    float angle = atan2(a[1], a[0]);

    return angle + (angle < 0) * 2 * M_PI;
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

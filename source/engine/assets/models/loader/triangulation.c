#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Vertex.h"

typedef float vec2[2];

enum PointState {
    REGULAR,
    START,
    END,
    SPLIT,
    MERGE
};

const char *names[] = {
    "REGULAR",
    "START",
    "END",
    "SPLIT",
    "MERGE"
};

bool isClockWise(size_t N, vec2 *v);
float getAngle(vec2 A, vec2 B, vec2 C);

static int doesIntersect(const vec2 p0, const vec2 p1, const vec2 p2, const vec2 p3) {
    vec2 s02 = {
        p0[0] - p2[0],
        p0[1] - p2[1]
    };
    vec2 s10 = {
        p1[0] - p0[0],
        p1[1] - p0[1]
    };
    vec2 s32 = {
        p3[0] - p2[0],
        p3[1] - p2[1]
    };

    float denom = s10[0] * s32[1] - s32[0] * s10[1];
    float s_numer = s10[0] * s02[1] - s10[1] * s02[0];
    float t_numer = s32[0] * s02[1] - s32[1] * s02[0];

    bool denomPositive = denom > 0;

    return (
        denom != 0 && // colinear
        (s_numer < 0) != denomPositive &&
        (t_numer < 0) != denomPositive &&
        (s_numer > denom) != denomPositive &&
        (t_numer > denom) != denomPositive
    ) ? 1 : 0;
}

struct Point {
    vec2 pos;
    int helpVal;
    uint16_t id;

    struct Point *next;
    struct Point *prev;

    struct Point *helper;
};

static bool less(const struct Point *p, const struct Point *q) {
    return p->pos[1] < q->pos[1] || p->pos[1] == q->pos[1] && p->pos[0] > q->pos[0];
}

static bool more(const struct Point *p, const struct Point *q) {
    return p->pos[1] > q->pos[1] || p->pos[1] == q->pos[1] && p->pos[0] < q->pos[0];
}

static int countIntersectionsRight(struct Point *this) {
    int count = 0;

    vec2 hiPos = {
        this->pos[0] + 10,
        this->pos[1]
    };

    struct Point *that = this;
    do {
        if (more(this, that) && less(this, that->next) || more(this, that->next) && less(this, that)) {
            count += this->pos[0] < that->pos[0] && this->pos[1] == that->pos[1] ? 1 :
                     this->pos[0] < that->next->pos[0] && this->pos[1] == that->next->pos[1] ? 1 : 
                     doesIntersect(this->pos, hiPos, that->pos, that->next->pos);
        }

        that = that->next;
    } while (that != this);

    return count;
}

static enum PointState getState(struct Point *this) {
    float angle = getAngle(this->prev->pos, this->pos, this->next->pos);

    printf("angle = %f =>", angle);

    return 
        (more(this, this->next) && more(this, this->prev)) ? (angle < M_PI ? START : SPLIT) :
        (less(this, this->next) && less(this, this->prev)) ? (angle < M_PI ? END   : MERGE) :
        REGULAR;
}

static float getXCoord(const struct Point *this, float y) {
    return this->pos[0] +
        (y - this->pos[1]) / 
        (this->next->pos[1] - this->pos[1]) * 
        (this->next->pos[0] - this->pos[0]);
}

static int comparePoints(const void *p, const void *q) {
    auto pp = *(const struct Point *const *)p;
    auto qq = *(const struct Point *const *)q;
    return 
        more(pp, qq) ? -1 :
        less(pp, qq) ?  1 :
                        0;
}

static void removePoint(size_t N, void **arr, void *toRemove) {
    int i = 0;

    while (arr[i] != toRemove) i += 1;

    arr[i] = arr[N - 1];
}

static size_t getLeft(size_t qT, struct Point **T, struct Point *this) {
    int max = -1;
    size_t i = 0;

    while (i < qT) {
        if (this->pos[0] > getXCoord(T[i], this->pos[1]) || this == T[i]->next) {
            if (max == -1 || getXCoord(T[max], this->pos[1]) < getXCoord(T[i], this->pos[1])) {
                max = i;
            }
        }

        i += 1;
    }

    assert(max != -1);

    return max;
}

static bool insideOnRight(struct Point *v) {
    return countIntersectionsRight(v) % 2 == 1;
}

void printPoint(const char *str, const struct Point *const v) {
    printf("\t%c%d%s\n",
        'A' + (v->id % ('Z' - 'A' + 1)),
        (v->id / ('Z' - 'A' + 1)),
        str
    );
    printf("\t\tPrev: %c%d\n",
        'A' + (v->prev->id % ('Z' - 'A' + 1)),
        (v->prev->id / ('Z' - 'A' + 1))
    );
    printf("\t\tNext %c%d\n",
        'A' + (v->next->id % ('Z' - 'A' + 1)),
        (v->next->id / ('Z' - 'A' + 1))
    );
}

void getProper(struct Point **lesser, struct Point **bigger) {
    struct Point **temp = lesser;
    struct Point *tempP;

    if (more(*lesser, *bigger)) {
        lesser = bigger;
        bigger = temp;
    }

    tempP = *lesser;
    while (more(tempP, *bigger) || less(tempP, *bigger)) tempP = tempP->next;

    *bigger = tempP;
}

static void MakeMonotone(size_t *outN, struct Point *polygon) {
    size_t N = *outN;
    struct Point *v[N]; {
        for (size_t i = 0; i < N; i += 1) {
            v[i] = &polygon[i];
        }
        qsort(v, N, sizeof(struct Point *), comparePoints);
    }

    struct Point *T[N];
    struct Point *toAdd[N];
    size_t qAdded = 0;
    size_t qT = 0;
    size_t j = 0;

    for (size_t i = 0; i < N; i += 1) {
        size_t temp = getState(v[i]);
        printf("%s", names[temp]);
        printPoint("", v[i]);
        switch(temp) {
            case REGULAR:
                if (insideOnRight(v[i])) {
                    if (getState(v[i]->prev->helper) == MERGE) {
                        toAdd[qAdded + 0] = v[i];
                        toAdd[qAdded + 1] = v[i]->prev->helper;

                        qAdded += 2;
                    }
                    removePoint(qT--, (void **)T, v[i]->prev);

                    (T[qT++] = v[i])->helper = v[i];
                }
                else {
                    j = getLeft(qT, T, v[i]);
                    if (getState(T[j]->helper) == MERGE) {
                        toAdd[qAdded + 0] = v[i];
                        toAdd[qAdded + 1] = T[j]->helper;

                        qAdded += 2;
                    }
                    T[j]->helper = v[i];
                }
                break;
            case END:
                if (getState(v[i]->prev->helper) == MERGE) {
                    toAdd[qAdded + 0] = v[i];
                    toAdd[qAdded + 1] = v[i]->prev->helper;

                    qAdded += 2;
                }
                removePoint(qT--, (void **)T, v[i]->prev);

                break;
            case SPLIT:
                j = getLeft(qT, T, v[i]);
                toAdd[qAdded + 0] = v[i];
                toAdd[qAdded + 1] = T[j]->helper;

                qAdded += 2;

                (T[qT++] = v[i])->helper = v[i];
                break;
            case START:
                (T[qT++] = v[i])->helper = v[i];

                break;
            case MERGE:
                if (getState(v[i]->prev->helper) == MERGE) {
                    toAdd[qAdded + 0] = v[i];
                    toAdd[qAdded + 1] = v[i]->prev->helper;

                    qAdded += 2;
                }
                removePoint(qT--, (void **)T, v[i]->prev);

                j = getLeft(qT, T, v[i]);
                if (getState(T[j]->helper) == MERGE) {
                    toAdd[qAdded + 0] = v[i];
                    toAdd[qAdded + 1] = T[j]->helper;

                    qAdded += 2;
                }
                T[j]->helper = v[i];

                break;
        }
    }

    for (size_t i = 0; i < qAdded; i += 2) {
        getProper(&toAdd[i + 0], &toAdd[i + 1]);

        polygon[N + 0] = (struct Point) {
            .pos = {
                [0] = toAdd[i + 0]->pos[0],
                [1] = toAdd[i + 0]->pos[1]
            },
            .prev = &polygon[N + 1],
            .next = toAdd[i + 0]->next,
            .helpVal = 0,
            .helper = toAdd[i + 0]->helper,
            .id = toAdd[i + 0]->id
        };

        polygon[N + 1] = (struct Point) {
            .pos = {
                [0] = toAdd[i + 1]->pos[0],
                [1] = toAdd[i + 1]->pos[1],
            },
            .prev = toAdd[i + 1]->prev,
            .next = &polygon[N + 0],
            .helpVal = 0,
            .helper = toAdd[i + 1]->helper,
            .id = toAdd[i + 1]->id
        };

        toAdd[i + 0]->next->prev = &polygon[N + 0];
        toAdd[i + 1]->prev->next = &polygon[N + 1];

        toAdd[i + 0]->next = toAdd[i + 1];
        toAdd[i + 1]->prev = toAdd[i + 0];

        printf("\t%c%d-%c%d\n",
            'A' + (toAdd[i + 0]->id % ('Z' - 'A' + 1)),
            (toAdd[i + 0]->id / ('Z' - 'A' + 1)),
            'A' + (toAdd[i + 1]->id % ('Z' - 'A' + 1)),
            (toAdd[i + 1]->id / ('Z' - 'A' + 1))
        );

        printPoint("1", toAdd[i + 0]);
        printPoint("1", toAdd[i + 1]);
        printPoint("2", &polygon[N + 0]);
        printPoint("2", &polygon[N + 1]);

        N += 2;
    }

    *outN += qAdded;
}

static bool isGood(int l, vec2 a, vec2 b, vec2 c) {
    vec2 arr[3] = {
        { a[0], a[1] },
        { b[0], b[1] },
        { c[0], c[1] },
    };

    return isClockWise(3, arr) ? l == 1 : l == 2;
}

static bool isLeft(vec2 a, vec2 b, vec2 c) {
    return (b[0] - a[0]) * (c[1] - a[1]) > (b[1] - a[1]) * (c[0] - a[0]);
}

static void TriangulateMonotone(size_t N, struct Point **u, uint16_t (**triangles)[3]) {
    size_t S[N];
    size_t top = 1;
    struct Point *c = NULL;

    qsort(u, N, sizeof(struct Point *), comparePoints);

    S[0] = 0;
    S[1] = 1;

    c = u[0]; while (c->next != u[N - 1]) {
        (c = c->next)->helpVal = 1;
        printf("L %c%d\n", 
            'A' + (c->id % ('Z' - 'A' + 1)),
            c->id / ('Z' - 'A' + 1)
        );
    }
    c = u[0]; while (c->prev != u[N - 1]) {
        (c = c->prev)->helpVal = 2;
        printf("R %c%d\n", 
            'A' + (c->id % ('Z' - 'A' + 1)),
            c->id / ('Z' - 'A' + 1)
        );
    }

    for (size_t j = 2; j < N - 1; j += 1) {
        // if (u[S[top]]->next != u[j] && u[S[top]]->prev != u[j]) {
        printf("Top = %d, J = %d, Triangle = ", u[S[top]]->helpVal, u[j]->helpVal);
        if (u[S[top]]->helpVal != u[j]->helpVal) {
            while (top > 0) {
                bool isLeftV = isLeft(u[j]->pos, u[S[top]]->pos, u[S[top - 1]]->pos);

                (**triangles)[0] = u[j]->id;
                (**triangles)[1] = u[S[top - (isLeftV ? 0 : 1)]]->id;
                (**triangles)[2] = u[S[top - (isLeftV ? 1 : 0)]]->id;

                printf("%c%d-%c%d-%c%d\n", 
                    'A' + ((**triangles)[0] % ('Z' - 'A' + 1)), 
                    ((**triangles)[0] / ('Z' - 'A' + 1)), 
                    'A' + ((**triangles)[1] % ('Z' - 'A' + 1)), 
                    ((**triangles)[1] / ('Z' - 'A' + 1)), 
                    'A' + ((**triangles)[2] % ('Z' - 'A' + 1)), 
                    ((**triangles)[2] / ('Z' - 'A' + 1))
                );

                *triangles += 1;

                top -= 1;
            }

            S[0] = j - 1;
            S[1] = j;

            top = 1;
        }
        else {
            while (top > 0 && isGood(u[j]->helpVal, u[j]->pos, u[S[top]]->pos, u[S[top - 1]]->pos)) {
                bool isLeftV = isLeft(u[j]->pos, u[S[top]]->pos, u[S[top - 1]]->pos);

                (**triangles)[0] = u[j]->id;
                (**triangles)[1] = u[S[top - (isLeftV ? 0 : 1)]]->id;
                (**triangles)[2] = u[S[top - (isLeftV ? 1 : 0)]]->id;

                printf("%c%d-%c%d-%c%d\n", 
                    'A' + ((**triangles)[0] % ('Z' - 'A' + 1)), 
                    ((**triangles)[0] / ('Z' - 'A' + 1)), 
                    'A' + ((**triangles)[1] % ('Z' - 'A' + 1)), 
                    ((**triangles)[1] / ('Z' - 'A' + 1)), 
                    'A' + ((**triangles)[2] % ('Z' - 'A' + 1)), 
                    ((**triangles)[2] / ('Z' - 'A' + 1))
                );

                *triangles += 1;

                top -= 1;
            }

            top += 1;
            S[top] = j;
        }
    }

    while (top > 0) {
        bool isLeftV = isLeft(u[N - 1]->pos, u[S[top]]->pos, u[S[top - 1]]->pos);

        (**triangles)[0] = u[N - 1]->id;
        (**triangles)[1] = u[S[top - (isLeftV ? 0 : 1)]]->id;
        (**triangles)[2] = u[S[top - (isLeftV ? 1 : 0)]]->id;

        printf("%c%d-%c%d-%c%d\n", 
            'A' + ((**triangles)[0] % ('Z' - 'A' + 1)), 
            ((**triangles)[0] / ('Z' - 'A' + 1)), 
            'A' + ((**triangles)[1] % ('Z' - 'A' + 1)), 
            ((**triangles)[1] / ('Z' - 'A' + 1)), 
            'A' + ((**triangles)[2] % ('Z' - 'A' + 1)), 
            ((**triangles)[2] / ('Z' - 'A' + 1))
        );

        *triangles += 1;

        top -= 1;
    }
}

static void TriangulatePolygon(size_t N, struct Point *polygon, uint16_t (*triangles)[3]) {
    struct Point *end;
    struct Point *monotone[N];
    size_t qTab = 0;

    for (size_t i = 0; i < N; i += 1) {
        if (polygon[i].helpVal == 0) {
            end = &polygon[i];
            qTab = 0;
            
            printf("AHA\n");
            do {
                (monotone[qTab++] = (end = end->next))->helpVal = 1;
                printPoint("", end);
            } while (end != &polygon[i]);
            printf("AHA\n");

            TriangulateMonotone(qTab, monotone, &triangles);

            i = -1;
        }
    }
}

#define OK(i) (isOk ? i : (vertexQuantity - 1 - i))

void triangulate(size_t vertexQuantity, size_t vertexIDs[vertexQuantity], struct FontVertex *vertex, uint16_t (*triangles)[3]) {
    vec2 verticies[vertexQuantity];
    for (size_t i = 0; i < vertexQuantity; i += 1) {
        verticies[i][0] = vertex[vertexIDs[i]].pos[0];
        verticies[i][1] = vertex[vertexIDs[i]].pos[1];
    }
    bool isOk = !isClockWise(vertexQuantity, verticies);

    struct Point polygon[vertexQuantity * 2];
    for (size_t i = 0; i < vertexQuantity; i += 1) {
        polygon[i] = (struct Point) {
            .helpVal = 0,
            .next = &polygon[(i + 1) % vertexQuantity],
            .prev = &polygon[(i - 1 + vertexQuantity) % vertexQuantity],
            .pos = {
                [0] = verticies[OK(i)][0],
                [1] = verticies[OK(i)][1]
            },
            .helper = NULL,
            .id = vertexIDs[OK(i)]
        };
    }

    for (size_t i = 0; i < vertexQuantity; i += 1) {
        printf("%zu\n",
            vertexIDs[i]
        );
    }
    for (size_t i = 0; i < vertexQuantity; i += 1) {
        printf("%c%d = (%f, %f)\n",
            (polygon[i].id % ('Z' - 'A' + 1)) + 'A',
            (polygon[i].id / ('Z' - 'A' + 1)),
            polygon[i].pos[0],
            polygon[i].pos[1]
        );
    }

    MakeMonotone(&vertexQuantity, polygon);
    printf("Aha\n");
    TriangulatePolygon(vertexQuantity, polygon, triangles);
}

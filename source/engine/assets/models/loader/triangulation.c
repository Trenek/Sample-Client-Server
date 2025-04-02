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

static int doesIntersect(vec2 p0, vec2 p1, vec2 p2, vec2 p3) {
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

static int countIntersections(struct Point *this, bool hi, vec2 hiPos) {
    int count = 0;
    int aaa = 0;

    struct Point *that = this->next;
    struct Point *end = this;
    while (hi ? (this->pos[0] == that->pos[0])
              : (this->pos[1] == that->pos[1])) {
        that = that->next;
    }
    while (hi ? (this->pos[0] == end->pos[0])
              : (this->pos[1] == end->pos[1])) {
        end = end->prev;
    }
    do {
        if (hi ? (this->pos[0] == that->pos[0] && this->pos[1] < that->pos[1])
               : (this->pos[0] < that->pos[0] && this->pos[1] == that->pos[1])) {
            aaa += 1;
        }
        else {
            int temp = doesIntersect(this->pos, hiPos, that->pos, that->next->pos);

            count += aaa + temp;
            aaa = 0;
        }

        that = that->next;
    } while (that != end && that->next != end);

    return count;
}

static bool less(const struct Point *p, const struct Point *q) {
    return p->pos[1] < q->pos[1] || (p->pos[1] == q->pos[1] && p->pos[0] > q->pos[0]);
}

static bool more(const struct Point *p, const struct Point *q) {
    return p->pos[1] > q->pos[1] || (p->pos[1] == q->pos[1] && p->pos[0] < q->pos[0]);
}

static enum PointState getState(struct Point *this) {
    vec2 hiPos = {
        this->pos[0],
        this->pos[1] + 1
    };

    return 
        (more(this, this->next) && more(this, this->prev)) ? (countIntersections(this, true, hiPos) % 2 == 0 ? START : SPLIT) :
        (less(this, this->next) && less(this, this->prev)) ? (countIntersections(this, true, hiPos) % 2 == 1 ? END   : MERGE) :
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
    vec2 rPos = {
        v->pos[0] + 10,
        v->pos[1]
    };

    return countIntersections(v, false, rPos) % 2 == 1;
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
        auto a = getState(v[i]);
        printf("%s - (%f, %f)\n", names[a], v[i]->pos[0], v[i]->pos[1]);
        switch(a) {
            case REGULAR:
                if (insideOnRight(v[i])) {
                    printf("\tPolygon On Right\n");
                    if (getState(v[i]->prev->helper) == MERGE) {
                        toAdd[qAdded + 0] = v[i];
                        toAdd[qAdded + 1] = v[i]->prev->helper;

                        qAdded += 2;
                    }
                    removePoint(qT--, (void **)T, v[i]->prev);

                    (T[qT++] = v[i])->helper = v[i];
                }
                else {
                    printf("\tPolygon On Left\n");
                    j = getLeft(qT, T, v[i]);
                    printf("\t%zu\n", qT);
                    for (size_t q = 0; q < qT; q += 1) {
                        printf("\t%f %f\n", T[q]->pos[0], T[q]->pos[1]);
                    }
                    printf("\t%zu\n", j);
                    if (getState(v[j]->helper) == MERGE) {
                        printf("\tAAAA\n");
                        toAdd[qAdded + 0] = v[i];
                        toAdd[qAdded + 1] = v[j]->helper;

                        qAdded += 2;
                    }
                    v[j]->helper = v[i];
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
                toAdd[qAdded + 1] = v[j]->helper;

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
                if (getState(v[j]->helper) == MERGE) {
                    toAdd[qAdded + 0] = v[i];
                    toAdd[qAdded + 1] = v[j]->helper;

                    qAdded += 2;
                }
                v[j]->helper = v[i];

                break;
        }
        printf("Good\n");
    }

    for (size_t i = 0; i < qAdded; i += 2) {
        polygon[N + 0] = (struct Point) {
            .pos = {
                [0] = toAdd[i + 0]->pos[0],
                [1] = toAdd[i + 0]->pos[1]
            },
            .prev = toAdd[i + 0]->prev,
            .next = toAdd[i + 1],
            .helpVal = 0,
            .helper = toAdd[i + 0]->helper,
            .id = toAdd[i + 0]->id
        };

        toAdd[i + 0]->prev->next = &polygon[N + 0];

        polygon[N + 1] = (struct Point) {
            .pos = {
                [0] = toAdd[i + 1]->pos[0],
                [1] = toAdd[i + 1]->pos[1],
            },
            .prev = toAdd[i + 1]->prev,
            .next = toAdd[i + 0],
            .helpVal = 0,
            .helper = toAdd[i + 1]->helper,
            .id = toAdd[i + 1]->id
        };

        toAdd[i + 1]->prev->next = &polygon[N + 1];

        N += 2;
    }

    *outN += qAdded;
}

static bool isClockWise(size_t N, vec2 *v) {
    float crossProductSum = 0.0f;
    
    for (size_t i = 0; i < N; i++) {
        size_t next = (i + 1) % N;
        size_t nextNext = (i + 2) % N;
        
        crossProductSum += 
            (v[next][0] - v[i][0]) * (v[nextNext][1] - v[i][1]) - 
            (v[next][1] - v[i][1]) * (v[nextNext][0] - v[i][0]);
    }
    
    return crossProductSum < 0;
}

static bool isClockWise2(vec2 v[3]) {
    return isClockWise(3, v);
}

static bool isGood(int l, vec2 a, vec2 b, vec2 c) {
    vec2 arr[3] = {
        { a[0], a[1] },
        { b[0], b[1] },
        { c[0], c[1] },
    };

    return isClockWise2(arr) ? l == 1 : l == 2;
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

    c = u[0]; while (c->next < c) (c = c->next)->helpVal = 1;
    c = u[0]; while (c->prev < c) (c = c->prev)->helpVal = 2;

    for (size_t j = 2; j < N - 1; j += 1) {
        if (u[S[top]]->next != u[j] && u[S[top]]->prev != u[j]) {
            while (top > 0) {
                bool isLeftV = isLeft(u[j]->pos, u[S[top]]->pos, u[S[top - 1]]->pos);

                (**triangles)[0] = u[j]->id;
                (**triangles)[1] = u[S[top - (isLeftV ? 0 : 1)]]->id;
                (**triangles)[2] = u[S[top - (isLeftV ? 1 : 0)]]->id;

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

        *triangles += 1;

        top -= 1;
    }
}

static void TriangulatePolygon(size_t N, struct Point *polygon, uint16_t triangles[18][3]) {
    struct Point *end;
    struct Point *monotone[N];
    size_t qTab = 0;

    for (size_t i = 0; i < N; i += 1) {
        if (polygon[i].helpVal == 0) {
            end = &polygon[i];
            qTab = 0;
            do {
                (monotone[qTab++] = (end = end->next))->helpVal = 1;
            } while (end != &polygon[i]);

            TriangulateMonotone(qTab, monotone, &triangles);

            i = -1;
        }
    }
}

void triangulate(size_t vertexQuantity, size_t vertexIDs[vertexQuantity], struct FontVertex *vertex, uint16_t (*triangles)[3]) {
    vec2 polygon[vertexQuantity];
    for (size_t i = 0; i < vertexQuantity; i += 1) {
        polygon[i][0] = vertex[vertexIDs[i]].pos[0];
        polygon[i][1] = vertex[vertexIDs[i]].pos[1];
    }
    vec2 *ready[vertexQuantity];
    size_t indexes[vertexQuantity];

    if (isClockWise(vertexQuantity, polygon)) {
        for (size_t i = 0; i < vertexQuantity; i += 1) {
            indexes[i] = vertexIDs[vertexQuantity - 1 - i];
            ready[i] = &polygon[vertexQuantity - 1 - i];
        }
    }
    else {
        for (size_t i = 0; i < vertexQuantity; i += 1) {
            indexes[i] = vertexIDs[i];
            ready[i] = &polygon[i];
        }
    }

    struct Point actualPolygon[vertexQuantity * 2];
    for (size_t i = 0; i < vertexQuantity; i += 1) {
        actualPolygon[i] = (struct Point) {
            .helpVal = 0,
            .next = &actualPolygon[(i + 1) % vertexQuantity],
            .prev = &actualPolygon[(i - 1 + vertexQuantity) % vertexQuantity],
            .pos = {
                [0] = (*ready[i])[0],
                [1] = (*ready[i])[1]
            },
            .helper = NULL,
            .id = indexes[i]
        };
    }

    for (size_t i = 0; i < vertexQuantity; i += 1) {
        printf("(%f, %f)\n", polygon[i][0], polygon[i][1]);
    }
    MakeMonotone(&vertexQuantity, actualPolygon);
    printf("Oho");
    TriangulatePolygon(vertexQuantity, actualPolygon, triangles);
}

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Vertex.h"

typedef float vec2[2];

enum PointState {
    REGULAR_INSIDE_ON_LEFT,
    REGULAR_INSIDE_ON_RIGHT,
    START,
    END,
    SPLIT,
    MERGE
};

const char *names[] = {
    "REGULAR_POLYGON_ON_LEFT",
    "REGULAR_POLYGON_ON_RIGHT",
    "START",
    "END",
    "SPLIT",
    "MERGE"
};

bool isCounterClockwise(size_t N, vec2 *v);
float getAngle(vec2 A, vec2 B, vec2 C);

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

static enum PointState getState(struct Point *this) {
    float angle = getAngle(this->prev->pos, this->pos, this->next->pos);

    return 
        (more(this, this->next) && more(this, this->prev)) ? (angle < M_PI ? START : SPLIT) :
        (less(this, this->next) && less(this, this->prev)) ? (angle < M_PI ? END   : MERGE) :
       // (more(this, this->prev) && less(this, this->next)) ? REGULAR_INSIDE_ON_RIGHT : REGULAR_INSIDE_ON_LEFT; not good because of some idiotic desicions
        !(more(this, this->prev) && less(this, this->next)) ? REGULAR_INSIDE_ON_RIGHT : REGULAR_INSIDE_ON_LEFT;
}

static float getXCoord(const struct Point *this, float y) {
    return this->pos[0] +
        (y - this->pos[1]) / 
        (this->next->pos[1] - this->pos[1]) * 
        (this->next->pos[0] - this->pos[0]);
}

static int comparePoints(const void *p, const void *q) {
    const struct Point *const *pp = p;
    const struct Point *const *qq = q;
    return 
        more(*pp, *qq) ? -1 :
        less(*pp, *qq) ?  1 :
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

bool isInBetween(vec2 A, vec2 B, vec2 C, vec2 D);

static void getProper(struct Point **lesser, struct Point **bigger, int N, struct Point *polygon) {
    struct Point **temp = lesser;

    if (more(*lesser, *bigger)) {
        lesser = bigger;
        bigger = temp;
    }

    while ((more(polygon, *bigger) || less(polygon, *bigger)) || 
            !isInBetween(polygon->prev->pos, polygon->pos, polygon->next->pos, (*lesser)->pos)) {
        polygon += 1;
        N -= 1;

        assert(N >= 0);
    }

    *bigger = polygon;
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
        switch (getState(v[i])) {
            case REGULAR_INSIDE_ON_RIGHT:
                if (getState(v[i]->prev->helper) == MERGE) {
                    toAdd[qAdded + 0] = v[i];
                    toAdd[qAdded + 1] = v[i]->prev->helper;

                    qAdded += 2;
                }
                removePoint(qT--, (void **)T, v[i]->prev);

                (T[qT++] = v[i])->helper = v[i];

                break;
            case REGULAR_INSIDE_ON_LEFT:
                j = getLeft(qT, T, v[i]);
                if (getState(T[j]->helper) == MERGE) {
                    toAdd[qAdded + 0] = v[i];
                    toAdd[qAdded + 1] = T[j]->helper;

                    qAdded += 2;
                }
                T[j]->helper = v[i];

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

                T[j]->helper = v[i];

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
        getProper(&toAdd[i + 0], &toAdd[i + 1], N, polygon);

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

    return isCounterClockwise(3, arr) ? l == 2 : l == 1;
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
    }
    c = u[0]; while (c->prev != u[N - 1]) {
        (c = c->prev)->helpVal = 2;
    }

    for (size_t j = 2; j < N - 1; j += 1) {
        // if (u[S[top]]->next != u[j] && u[S[top]]->prev != u[j]) {
        if (u[S[top]]->helpVal != u[j]->helpVal) {
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

static void TriangulatePolygon(size_t N, struct Point *polygon, uint16_t (*triangles)[3]) {
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

static size_t sum(int q, size_t arr[q]) {
    size_t result = 0;

    while (q > 0) {
        q -= 1;

        result += arr[q];
    }

    return result;
}

void triangulate(size_t q, size_t vertexQuantity[q], size_t *vertexIDs[q], struct FontVertex *vertex, uint16_t (*triangles)[3]) {
    struct Point polygon[2 * sum(q, vertexQuantity)];
    size_t actualVertexQuantity = 0;

    for (size_t i = 0; i < q; i += 1) {
        for (size_t j = 0; j < vertexQuantity[i]; j += 1) {
            polygon[actualVertexQuantity + j] = (struct Point) {
                .helpVal = 0,
                .next = &polygon[actualVertexQuantity + ((j + 1) % vertexQuantity[i])],
                .prev = &polygon[actualVertexQuantity + ((j - 1 + vertexQuantity[i]) % vertexQuantity[i])],
                .pos = {
                    [0] = vertex[vertexIDs[i][j]].pos[0],
                    [1] = vertex[vertexIDs[i][j]].pos[1]
                },
                .helper = NULL,
                .id = vertexIDs[i][j]
            };
        }

        actualVertexQuantity += vertexQuantity[i];
    }

    MakeMonotone(&actualVertexQuantity, polygon);
    TriangulatePolygon(actualVertexQuantity, polygon, triangles);
}

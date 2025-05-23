#include "state.h"

typedef float vec3[3];

struct player {
    struct player *enemy;

    struct Entity *model;
    struct actualModel *actualModel;

    // w, a, s, d, x, y, a, b, left, right
    int playerKeys[10];
    int playerJoystick;

    int maxHealth;
    int currentHealth;
    double *healthPercentage;

    float maxRest;
    float currentRest;
    double *restPercentage;

    bool hitLock;
    bool hurtLock;
    bool doesHitLast;
    bool didHit;
    float hitTime;
    float hurtTime;

    float time;
    int state;

    struct camera *splitScreen;
    struct camera *face;

    vec3 relativeFaceCameraPos;
};

enum animation {
    CHARGE,
    DAMAGE_HIT,
    DAMAGE_KICK,
    LEFT_KICK,
    LEFT_LOW_PUNCH,
    LEFT_HIGH_PUNCH,
    RIGHT_KICK,
    RIGHT_LOW_PUNCH,
    RIGHT_HIGH_PUNCH,
    STANDING,
    WALK,

    FAST_WALK,
};

struct WindowManager;
void movePlayer(struct player *p, struct WindowManager *wc, float deltaTime, enum state *state);
void moveEnemy(struct player *p, struct WindowManager *, float deltaTime);
void posePlayer(struct player *p, float deltaTime);

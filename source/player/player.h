typedef float vec3[3];

struct player {
    struct player *enemy;

    struct Entity *model;
    struct actualModel *actualModel;

    int playerKeys[10];
    int playerJoystick;

    int maxHealth;
    int currentHealth;
    double *healthPercentage;

    int maxRest;
    int currentRest;
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
    BATTLE_WALK,
    DAMAGE_HIT,
    DAMAGE_KICK,
    LEFT_KICK,
    LEFT_LOW_PUNCH,
    LEFT_HIGH_PUNCH,
    RIGHT_KICK,
    RIGHT_LOW_PUNCH,
    RIGHT_HIGH_PUNCH,
    STANDING,
    WALK
};

struct WindowManager;
void movePlayer(struct player *p, struct WindowManager *wc, float deltaTime);
void moveEnemy(struct player *p, struct WindowManager *, float deltaTime);
void posePlayer(struct player *p, float deltaTime);

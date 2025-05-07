struct player {
    struct Entity *model;
    struct actualModel *actualModel;

    float time;
    int state;
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

struct windowControl;
void movePlayer(struct player *p, struct windowControl *wc, float deltaTime);
void moveEnemy(struct player *p, struct windowControl *, float deltaTime);

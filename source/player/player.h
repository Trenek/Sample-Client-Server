struct player {
    struct Entity *model;
    struct actualModel *actualModel;

    float time;
    int state;
};

struct windowControl;
void movePlayer(struct player *p, struct windowControl *wc, float deltaTime);

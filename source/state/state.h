#ifndef STATE_H
#define STATE_H

struct EngineCore;
enum state {
    MAIN_MENU,
    GAME,
    PAUSE,
    WIN,
    LOSE,
    EXIT
};

void game(struct EngineCore *engine, enum state *state);
void menu(struct EngineCore *engine, enum state *state);

#endif

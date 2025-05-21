#ifndef STATE_H
#define STATE_H

#include <stdint.h>

enum state {
    LOAD_MENU,
    MAIN_MENU,
    GAME,
    PAUSE,
    WIN_SCREEN,
    LOSE,
    EXIT
};

struct EngineCore;
struct renderPassObj;

void game(struct EngineCore *engine, enum state *state);
void menu(struct EngineCore *engine, enum state *state);
void win(struct EngineCore *engine, enum state *state);
void loadMenu(struct EngineCore *engine, enum state *state);

#endif

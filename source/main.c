#include "engineCore.h"

#include "state.h"

int main() {
    struct EngineCore engine = setup();
    void (* const state[])(struct EngineCore *engine, enum state *state) = {
        [GAME] = game,
        [LOAD_GAME] = loadGame,
        [LOAD_RESOURCES] = loadResources,
        [MAIN_MENU] = menu,
        [WIN_SCREEN] = win,
        [PAUSE] = pause
    };
    enum state stateID = LOAD_RESOURCES;

    do {
        state[stateID](&engine, &stateID);
    } while (stateID != EXIT && !shouldWindowClose(engine.window));

    cleanup(engine);

    return 0;
}

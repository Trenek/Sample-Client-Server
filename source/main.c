#include "engineCore.h"

#include "state.h"

int main() {
    struct EngineCore engine = setup();
    void (* const state[])(struct EngineCore *engine, enum state *state) = {
        [GAME] = game,
        [MAIN_MENU] = menu
    };
    enum state stateID = MAIN_MENU;

    loadSound(&engine.soundManager, 0, "music/music.mp3");
    playSound(&engine.soundManager, 0, true);

    do {
        state[stateID](&engine, &stateID);
    } while (stateID != EXIT && !shouldWindowClose(engine.window));

    cleanup(engine);

    return 0;
}

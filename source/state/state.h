#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "model.h"

struct VulkanTools;
enum state {
    MAIN_MENU,
    GAME,
    PAUSE,
    WIN,
    LOSE,
    EXIT
};

void game(struct VulkanTools *vulkan, enum state *state);

#endif

#include <stdlib.h>
#include <time.h>

#include "VulkanTools.h"

#include "state.h"

int main() {
    struct VulkanTools vulkan = setup();
    void (* const state[])(struct VulkanTools *vulkan, enum state *state) = {
        [GAME] = game
    };
    enum state stateID = GAME;

    srand(time(NULL));
    do {
        state[stateID](&vulkan, &stateID);
    } while (stateID != EXIT && !glfwWindowShouldClose(vulkan.window));

    cleanup(vulkan);

    return 0;
}

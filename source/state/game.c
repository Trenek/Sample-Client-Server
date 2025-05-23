#include <cglm.h>
#include <string.h>

#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "instanceBuffer.h"

#include "graphicsPipelineObj.h"
#include "renderPass.h"

#include "player.h"

void game(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *screenData = findResource(&engine->resource, "ScreenData");

    struct Entity *entity[] = {
        findResource(entityData, "Floor"),
        findResource(entityData, "Player 1"),
        findResource(entityData, "Player 2"),
        findResource(entityData, "Fight!"),
        findResource(entityData, "Background"),
        findResource(entityData, "Player 1 Text"),
        findResource(entityData, "Player 2 Text"),
        findResource(entityData, "Blue Back"),
        findResource(entityData, "Health"),
        findResource(entityData, "Health Background"),
        findResource(entityData, "Rest"),
        findResource(entityData, "Rest Background"),
        findResource(entityData, "Cube")
    };
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);
    
    struct renderPassObj *renderPass[] = {
        findResource(screenData, "Left Screen"),
        findResource(screenData, "Right Screen"),
        findResource(screenData, "Background Left 1"),
        findResource(screenData, "Background Right 1"),
        findResource(screenData, "Middle Text"),
        findResource(screenData, "Left Figure"),
        findResource(screenData, "Right Figure"),
        findResource(screenData, "Left Text"),
        findResource(screenData, "Right Text"),
        findResource(screenData, "Background Left 2"),
        findResource(screenData, "Background Left 3"),
        findResource(screenData, "Background Left 4"),
        findResource(screenData, "Background Left 5"),
        findResource(screenData, "Background Right 2"),
        findResource(screenData, "Background Right 3"),
        findResource(screenData, "Background Right 4"),
        findResource(screenData, "Background Right 5"),
    };

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct player *playerData = findResource(&engine->resource, "actualPlayerData");

    struct playerInstance *player = entity[1]->instance;
    struct playerInstance *enemy = entity[2]->instance;
    struct instance *text = entity[3]->instance;

    playSound(&engine->soundManager, 1, true, 1.0f);

    while (GAME == *state && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        movePlayer(&playerData[0], &engine->window, engine->deltaTime.deltaTime, state);
        movePlayer(&playerData[1], &engine->window, engine->deltaTime.deltaTime, state);

        drawFrame(engine, qRenderPass, renderPass);

        bool isMClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(&engine->window, GLFW_KEY_M);
        bool isPClicked = (KEY_PRESS | KEY_CHANGE) == getKeyState(&engine->window, GLFW_KEY_P);

        if (isMClicked) text->shadow = !text->shadow;
        if (isPClicked) *state = PAUSE;
        if (playerData[0].currentHealth <= 0 || playerData[1].currentHealth <= 0) {
            *state = WIN_SCREEN;
        }
    }

    switch (*state) {
        case WIN_SCREEN: {
            struct playerInstance *info = malloc(sizeof(struct playerInstance)); {
                memcpy(info, playerData[0].currentHealth <= 0 ? enemy : player, sizeof(struct playerInstance));
            }
            char *name = malloc(sizeof("Player 1")); {
                memcpy(name, "Player 1", sizeof("Player 1"));
                if (playerData[0].currentHealth <= 0) {
                    name[7] = '2';
                }
            }
            char *numb = malloc(sizeof(char)); {
                *numb = playerData[0].currentHealth <= 0 ? GLFW_JOYSTICK_2 : GLFW_JOYSTICK_1;
            }

            addResource(&engine->resource, "playerInfo", info, free);
            addResource(&engine->resource, "playerName", name, free);
            addResource(&engine->resource, "playerNumb", numb, free);
            break;
        }
        case PAUSE: {
            char *numb = malloc(sizeof(char)); {
                *numb = playerData[0].currentHealth <= 0 ? GLFW_JOYSTICK_2 : GLFW_JOYSTICK_1;
            }

            addResource(&engine->resource, "playerNumb", numb, free);
        }
        default:
    };

    switch (*state) {
        case PAUSE:
            break;
        default:
            vkDeviceWaitIdle(engine->graphics.device);
            cleanupResource(&engine->resource, "ScreenData");
            cleanupResource(&engine->resource, "actualPlayerData");
    };
}

#include "windowControl.h"

uint8_t getKeyState(struct windowControl *windowControl, int key) {
    uint8_t result = (windowControl->key[key].press ? KEY_PRESS : KEY_RELEASE) |
                     (windowControl->key[key].change ? KEY_CHANGE : KEY_REPEAT);
    windowControl->key[key].change = 0;

    return result;
}

uint8_t getMouseState(struct windowControl *windowControl, int key) {
    uint8_t result = (windowControl->mouseButton[key].press ? KEY_PRESS : KEY_RELEASE) |
           (windowControl->mouseButton[key].change ? KEY_CHANGE : KEY_REPEAT);

    windowControl->mouseButton[key].change = 0;

    return result;
}

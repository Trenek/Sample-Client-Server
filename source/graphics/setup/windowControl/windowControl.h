#include <GLFW/glfw3.h>

union keyState {
    struct {
        uint8_t press : 1;
        uint8_t change : 1;
        uint8_t : 6;
    };
    uint8_t val;
};

struct windowControl {
    union keyState mouseButton[GLFW_MOUSE_BUTTON_LAST + 1];
    union keyState key[GLFW_KEY_LAST + 1];
};

enum keyStateNum {
    KEY_RELEASE = 1 << 0,
    KEY_PRESS =   1 << 1,
    KEY_REPEAT =  1 << 2,
    KEY_CHANGE =  1 << 3
};

uint8_t getKeyState(struct windowControl *windowControl, int key);
uint8_t getMouseState(struct windowControl *windowControl, int key);

#include <GLFW/glfw3.h>

union keyState {
    struct {
        uint8_t press : 1;
        uint8_t change : 1;
        uint8_t : 6;
    };
    uint8_t val;
};

struct windowData {
    bool framebufferResized;

    union keyState mouseButton[GLFW_MOUSE_BUTTON_LAST + 1];
    union keyState key[GLFW_KEY_LAST + 1];
    unsigned char gamepad[GLFW_GAMEPAD_BUTTON_LAST + 1];
    float state[GLFW_GAMEPAD_AXIS_LAST + 1];
};

struct WindowManager {
    GLFWwindow *window;

    struct windowData *data;
};

enum keyStateNum {
    KEY_RELEASE = 1 << 0,
    KEY_PRESS =   1 << 1,
    KEY_REPEAT =  1 << 2,
    KEY_CHANGE =  1 << 3
};

struct WindowManager initWindowManager(void);
void cleanupWindowControl(struct WindowManager this);

uint8_t getKeyState(struct WindowManager *windowControl, int key);
uint8_t getMouseState(struct WindowManager *windowControl, int key);
bool shouldWindowClose(struct WindowManager windowControl);

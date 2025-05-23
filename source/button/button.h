struct Button {
    int joystick;
    bool isClicked;
    int qButton;
    struct Entity *entity;
    struct actualModel *model;
    struct CameraBuffer *camera;
    int (*newState);

    int chosen;
};

struct GraphicsSetup;
struct WindowManager;
void shadowButton(struct GraphicsSetup gs, struct WindowManager wm, struct Button *button);

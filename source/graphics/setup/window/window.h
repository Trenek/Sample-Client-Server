#include <GLFW/glfw3.h>

GLFWwindow *createWindow(bool **framebufferResizedPtr, struct windowControl **pWindowControl);
void destroyWindow(GLFWwindow *window);

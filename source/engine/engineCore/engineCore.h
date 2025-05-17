#ifndef ENGINE_CORE_H
#define ENGINE_CORE_H

#include "windowManager.h"
#include "deltaTime.h"
#include "soundManager.h"

#include "graphicsSetup.h"

struct EngineCore {
    struct WindowManager window;
    struct DeltaTimeManager deltaTime;
    struct SoundManager soundManager;

    struct GraphicsSetup graphics;
};

struct EngineCore setup();
void recreateSwapChain(struct EngineCore *vulkan);
void cleanup(struct EngineCore vulkan);

struct renderPassObj;
void drawFrame(struct EngineCore *vulkan, uint16_t qRenderPass, struct renderPassObj *renderPass);

#endif

#include <time.h>

struct deltaTimeManager {
    struct timespec prev;
    struct timespec curr;

    float deltaTime;
};

struct deltaTimeManager initDeltaTime();
void updateDeltaTime(struct deltaTimeManager *deltaTime);
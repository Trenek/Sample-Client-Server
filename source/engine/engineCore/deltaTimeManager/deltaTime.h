#include <time.h>

struct DeltaTimeManager {
    struct timespec prev;
    struct timespec curr;

    float deltaTime;
};

struct DeltaTimeManager initDeltaTimeManager(void);
void updateDeltaTime(struct DeltaTimeManager *deltaTime);

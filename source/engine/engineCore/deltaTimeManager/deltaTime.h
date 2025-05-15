#include <time.h>

struct DeltaTimeManager {
    struct timespec prev;
    struct timespec curr;

    float deltaTime;
};

struct DeltaTimeManager initDeltaTime(void);
void updateDeltaTime(struct DeltaTimeManager *deltaTime);

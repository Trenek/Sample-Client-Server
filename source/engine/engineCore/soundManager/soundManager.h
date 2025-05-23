#include <miniaudio.h>

struct SoundManager {
    ma_engine *engine;

    size_t qSound;
    ma_sound *prevSound;
    ma_sound **sound;
};

struct SoundManager initSoundManager(void);
void cleanupSoundManager(struct SoundManager this);

void loadSound(struct SoundManager *this, size_t soundIndex, const char *soundName);
void playSound(struct SoundManager *this, size_t soundIndex, bool shouldLoop, float volume);

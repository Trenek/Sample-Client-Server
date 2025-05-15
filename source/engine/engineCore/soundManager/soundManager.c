#include <assert.h>
#include <stdlib.h>

#include "soundManager.h"

struct SoundManager initSoundManager(void) {
    struct SoundManager result = {
        .engine = malloc(sizeof(ma_engine)),
        .qSound = 0,
        .sound = NULL
    };

    assert(MA_SUCCESS == ma_engine_init(NULL, result.engine));

    return result;
}

void loadSound(struct SoundManager *this, size_t soundIndex, const char *soundName) {
    if (this->qSound < soundIndex + 1) {
        this->sound = realloc(this->sound, sizeof(ma_sound) * (soundIndex + 1));
    }

    assert(MA_SUCCESS == ma_sound_init_from_file(this->engine, soundName, 0, NULL, NULL, &this->sound[soundIndex]));
}

void playSound(struct SoundManager *this, size_t soundIndex, bool shouldLoop) {
    ma_sound_start(&this->sound[soundIndex]);
    ma_sound_set_looping(&this->sound[soundIndex], shouldLoop);
}

void cleanupSoundManager(struct SoundManager this) {
    for (size_t i = 0; i < this.qSound; i += 1) {
        ma_sound_uninit(&this.sound[i]);
    }

    ma_engine_uninit(this.engine);

    free(this.sound);
    free(this.engine);
}

#include <iostream>
#include "Audio.h"
#include "miniaudio.h"  // solo las declaraciones; la implementación está en AudioLibrary.cpp

// Archivos de cada pista y efecto, en el orden de los enums de Audio.h.
static const char* MUSIC_FILES[Audio::MUSIC_COUNT] = {
    nullptr,                            // NO_MUSIC
    "sounds/music/menu.ogg",
    "sounds/music/level.ogg",
    "sounds/music/boss.ogg",
    "sounds/music/level-clear.ogg",
    "sounds/music/game-over.ogg",
    "sounds/music/game-completed.ogg",
};
static const char* SOUND_FILES[Audio::SOUND_COUNT] = {
    "sounds/sfx/bomb-place.ogg",
    "sounds/sfx/bomb_explosion.wav",
    "sounds/sfx/jump.wav",
    "sounds/sfx/get-powerup.wav",
    "sounds/sfx/shooter.wav",
    "sounds/sfx/player-hit.wav",
    "sounds/sfx/enemy-death.wav",
    "sounds/sfx/open-door.wav",
    "sounds/sfx/statue-land.wav",
    "sounds/sfx/boss-hurt.wav",
    "sounds/sfx/Boss-tp.wav",
    "sounds/sfx/boss-shoots.wav",
    "sounds/sfx/menu-hover.wav",
    "sounds/sfx/menuselect.ogg",
};
static const float MUSIC_VOLUME = 0.4f; 
static const float SOUND_VOLUME = 0.7f;

struct Audio::Impl {
    ma_engine engine;
    // Cada efecto se carga una vez, ya descodificado, para que suene sin retardo.
    ma_sound sounds[SOUND_COUNT];
    bool soundLoaded[SOUND_COUNT] = {};
    // La música se lee del disco poco a poco (stream): los .ogg son largos.
    ma_sound music;
    bool musicLoaded = false;
    Music musicId = NO_MUSIC;
    bool musicLoops = false;
};

Audio& Audio::instance() {
    static Audio audio;
    return audio;
}

void Audio::init(bool enabled) {
    if (!enabled || impl != nullptr) return;
    impl = new Impl();
    if (ma_engine_init(nullptr, &impl->engine) != MA_SUCCESS) {
        std::cerr << "Audio: no se ha podido abrir el dispositivo; el juego será mudo" << std::endl;
        delete impl;
        impl = nullptr;
        return;
    }
    for (int i = 0; i < SOUND_COUNT; ++i) {
        impl->soundLoaded[i] = ma_sound_init_from_file(&impl->engine, SOUND_FILES[i],
            MA_SOUND_FLAG_DECODE, nullptr, nullptr, &impl->sounds[i]) == MA_SUCCESS;
        if (impl->soundLoaded[i]) ma_sound_set_volume(&impl->sounds[i], SOUND_VOLUME);
        else std::cerr << "Audio: falta " << SOUND_FILES[i] << std::endl;
    }
}

void Audio::shutdown() {
    if (impl == nullptr) return;
    stopMusic();
    for (int i = 0; i < SOUND_COUNT; ++i)
        if (impl->soundLoaded[i]) ma_sound_uninit(&impl->sounds[i]);
    ma_engine_uninit(&impl->engine);
    delete impl;
    impl = nullptr;
}

void Audio::playMusic(Music music, bool loop) {
    if (impl == nullptr || music == currentMusic()) return;
    stopMusic();
    if (music == NO_MUSIC) return;
    if (ma_sound_init_from_file(&impl->engine, MUSIC_FILES[music], MA_SOUND_FLAG_STREAM,
            nullptr, nullptr, &impl->music) != MA_SUCCESS) {
        std::cerr << "Audio: falta " << MUSIC_FILES[music] << std::endl;
        return;
    }
    impl->musicLoaded = true;
    impl->musicId = music;
    impl->musicLoops = loop;
    ma_sound_set_looping(&impl->music, loop ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&impl->music, MUSIC_VOLUME);
    ma_sound_start(&impl->music);
}

void Audio::stopMusic() {
    if (impl == nullptr || !impl->musicLoaded) return;
    ma_sound_uninit(&impl->music);  // también la para
    impl->musicLoaded = false;
    impl->musicId = NO_MUSIC;
}

void Audio::stopJingle() {
    if (isJinglePlaying()) stopMusic();
}

Audio::Music Audio::currentMusic() const {
    if (impl == nullptr || !impl->musicLoaded) return NO_MUSIC;
    // Un jingle que ya ha llegado al final cuenta como silencio.
    if (!impl->musicLoops && ma_sound_at_end(&impl->music)) return NO_MUSIC;
    return impl->musicId;
}

bool Audio::isJinglePlaying() const {
    return currentMusic() != NO_MUSIC && !impl->musicLoops;
}

void Audio::play(Sound sound) {
    if (impl == nullptr || !impl->soundLoaded[sound]) return;
    ma_sound_seek_to_pcm_frame(&impl->sounds[sound], 0);
    ma_sound_start(&impl->sounds[sound]);
}

#include "AudioManager.h"
#include <cstdio>

AudioManager::AudioManager()
    : eatSound(nullptr)
    , gameOverSound(nullptr)
    , bgMusic(nullptr)
    , initialized(false)
    , muted(false)
    , musicVolume(Constants::MUSIC_VOLUME)
    , sfxVolume(Constants::SFX_VOLUME) {
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::init() {
    // Initialize SDL_mixer
    if (Mix_OpenAudio(Constants::AUDIO_FREQUENCY,
                      MIX_DEFAULT_FORMAT,
                      Constants::AUDIO_CHANNELS,
                      Constants::AUDIO_CHUNK_SIZE) < 0) {
        printf("Warning: Could not initialize audio: %s\n", Mix_GetError());
        printf("Game will run without sound.\n");
        return false;
    }

    // Load sound effects
    eatSound = Mix_LoadWAV(Constants::EAT_SOUND_PATH);
    if (!eatSound) {
        printf("Warning: Could not load eat sound: %s\n", Mix_GetError());
    }

    gameOverSound = Mix_LoadWAV(Constants::GAMEOVER_SOUND_PATH);
    if (!gameOverSound) {
        printf("Warning: Could not load game over sound: %s\n", Mix_GetError());
    }

    // Load background music
    bgMusic = Mix_LoadMUS(Constants::BGM_PATH);
    if (!bgMusic) {
        printf("Warning: Could not load background music: %s\n", Mix_GetError());
    }

    // Set initial volumes
    Mix_VolumeMusic(musicVolume);

    initialized = true;
    return true;
}

void AudioManager::shutdown() {
    if (!initialized) return;

    // Stop all audio
    Mix_HaltMusic();
    Mix_HaltChannel(-1);

    // Free sound effects
    if (eatSound) {
        Mix_FreeChunk(eatSound);
        eatSound = nullptr;
    }

    if (gameOverSound) {
        Mix_FreeChunk(gameOverSound);
        gameOverSound = nullptr;
    }

    // Free music
    if (bgMusic) {
        Mix_FreeMusic(bgMusic);
        bgMusic = nullptr;
    }

    // Close audio
    Mix_CloseAudio();
    initialized = false;
}

void AudioManager::playEatSound() {
    if (!initialized || muted || !eatSound) return;

    Mix_VolumeChunk(eatSound, sfxVolume);
    Mix_PlayChannel(-1, eatSound, 0);
}

void AudioManager::playGameOverSound() {
    if (!initialized || muted || !gameOverSound) return;

    Mix_VolumeChunk(gameOverSound, sfxVolume);
    Mix_PlayChannel(-1, gameOverSound, 0);
}

void AudioManager::playBackgroundMusic() {
    if (!initialized || !bgMusic) return;

    if (!muted) {
        Mix_VolumeMusic(musicVolume);
    } else {
        Mix_VolumeMusic(0);
    }

    // Play music with infinite loop (-1)
    if (Mix_PlayMusic(bgMusic, -1) < 0) {
        printf("Warning: Could not play music: %s\n", Mix_GetError());
    }
}

void AudioManager::stopBackgroundMusic() {
    if (!initialized) return;
    Mix_HaltMusic();
}

void AudioManager::pauseBackgroundMusic() {
    if (!initialized) return;
    Mix_PauseMusic();
}

void AudioManager::resumeBackgroundMusic() {
    if (!initialized) return;
    Mix_ResumeMusic();
}

bool AudioManager::isMusicPlaying() const {
    if (!initialized) return false;
    return Mix_PlayingMusic() != 0;
}

void AudioManager::setMusicVolume(int volume) {
    musicVolume = (volume < 0) ? 0 : (volume > 128) ? 128 : volume;
    if (initialized && !muted) {
        Mix_VolumeMusic(musicVolume);
    }
}

void AudioManager::setSfxVolume(int volume) {
    sfxVolume = (volume < 0) ? 0 : (volume > 128) ? 128 : volume;
}

void AudioManager::toggleMute() {
    muted = !muted;

    if (initialized) {
        if (muted) {
            Mix_VolumeMusic(0);
        } else {
            Mix_VolumeMusic(musicVolume);
        }
    }
}

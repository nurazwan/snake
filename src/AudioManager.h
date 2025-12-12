#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL2/SDL_mixer.h>
#include <string>
#include "Constants.h"

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    // Initialize audio system
    bool init();

    // Shutdown audio system
    void shutdown();

    // Play sound effects
    void playEatSound();
    void playGameOverSound();

    // Background music controls
    void playBackgroundMusic();
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();
    bool isMusicPlaying() const;

    // Volume controls (0-128)
    void setMusicVolume(int volume);
    void setSfxVolume(int volume);

    // Mute controls
    void toggleMute();
    bool isMuted() const { return muted; }

private:
    // Sound effects
    Mix_Chunk* eatSound;
    Mix_Chunk* gameOverSound;

    // Background music
    Mix_Music* bgMusic;

    // State
    bool initialized;
    bool muted;
    int musicVolume;
    int sfxVolume;
};

#endif // AUDIOMANAGER_H

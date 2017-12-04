#ifndef AUDIOMANAGER_H_INCLUDED
#define AUDIOMANAGER_H_INCLUDED

#include <SDL2/SDL_mixer.h>
#include <map>

#include "MyEngine.h"

namespace nta {
    class AudioManager;
    ///Represents a sound effect or short audio clip
    class SoundEffect {
    private:
        ///Constructor simple stores a chunk
        SoundEffect(Mix_Chunk* c) : m_chunk(c) {
        }
        ///Destructor frees chunk
        ~SoundEffect() {
            if (m_chunk) {
                Mix_FreeChunk(m_chunk);
            }
        }
        ///the chunk representing the stored sound effect
        Mix_Chunk* m_chunk;
    public:
        ///plays the sound effect
        void play(int numLoops = 0) const {
            Mix_PlayChannel(-1, m_chunk, numLoops);
        }
        friend class AudioManager;
    };
    ///Represents a longer piece of music
    class Music {
    private:
        ///constructor simply stores music
        Music(Mix_Music* m) : m_music(m) {
        }
        ///destructor frees stored music
        ~Music() {
            if (m_music) {
                Mix_FreeMusic(m_music);
            }
        }
        ///the stored music
        Mix_Music* m_music = nullptr;
    public:
        ///plays music
        void play(int numLoops = 1) const {
            Mix_PlayMusic(m_music, numLoops);
        }
        ///pauses music (can be resumed)
        void pause() const {
            Mix_PauseMusic();
        }
        ///stops music (must be replayed from beginning)
        void stop() const {
            Mix_HaltMusic();
        }
        ///resumes paused music
        void resume() const {
            Mix_ResumeMusic();
        }
        friend class AudioManager;
    };
    class AudioManager {
    private:
        ///maps for associating sounds with their files
        static std::map<std::string, SoundEffect*> m_effectMap;
        static std::map<std::string, Music*> m_musicMap;
    public:
        ///initializes SDL_Mixer
        static void init();
        ///frees all music
        static void destroy();
        ///returns sound
        static SoundEffect* getSoundEffect(crstring effectPath);
        static Music* getMusic(crstring musicPath);
    };
}

#endif // AUDIOMANAGER_H_INCLUDED

#include "AudioManager.h"
#include "Logger.h"

namespace nta {
    std::map<std::string, SoundEffect*>     AudioManager::m_effectMap;
    std::map<std::string, Music*>           AudioManager::m_musicMap;
    void AudioManager::init() {
        Logger::writeToLog("Initializing AudioManager...");
        if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == -1) {
            Logger::writeErrorToLog("Failed to initialize SDL_mixer: " + toString(Mix_GetError()));
        }
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
            Logger::writeErrorToLog("Failed to open audio: " + toString(Mix_GetError()));
        }
        Logger::writeToLog("Initialized AudioManager");
    }
    void AudioManager::destroy() {
        for (auto& it : m_effectMap) {
            delete it.second;
        }
        for (auto& it : m_musicMap) {
            delete it.second;
        }
        m_effectMap.clear();
        m_musicMap.clear();
        Mix_CloseAudio();
        Mix_Quit();
    }
    SoundEffect* AudioManager::getSoundEffect(crstring effectPath) {
        std::string folder = "";
        if (effectPath.find("resources/sound effects/") != 0) {
            folder = "resources/sound effects/";
        }
        if (m_effectMap.find(folder+effectPath) == m_effectMap.end()) {
            nta::Logger::writeToLog("Loading sound effect: " + folder + effectPath + "...");
            Mix_Chunk* newChunk = Mix_LoadWAV((folder+effectPath).c_str());
            if (!newChunk) {
                nta::Logger::writeErrorToLog("Unable to load sound effect: " + toString(Mix_GetError()));
            }
            m_effectMap[folder+effectPath] = new SoundEffect(newChunk);
            nta::Logger::writeToLog("Loaded sound effect");
        }
        return m_effectMap[folder+effectPath];
    }
    Music* AudioManager::getMusic(crstring musicPath) {
        std::string folder = "";
        if (musicPath.find("resources/music/") != 0) {
            folder = "resources/music/";
        }
        if (m_musicMap.find(folder+musicPath) == m_musicMap.end()) {
            nta::Logger::writeToLog("Loading music file: " + folder + musicPath + "...");
            Mix_Music* newMusic = Mix_LoadMUS((folder+musicPath).c_str());
            if (!newMusic) {
                nta::Logger::writeErrorToLog("Unable to load music file: " + toString(Mix_GetError()));
            }
            m_musicMap[folder+musicPath] = new Music(newMusic);
            nta::Logger::writeToLog("Loaded music file");
        }
        return m_musicMap[folder+musicPath];
    }
};
